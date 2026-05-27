#pragma once
// 一个最小可用的 HTTP/1.1 服务器：单线程、阻塞、按请求处理。
// 只用于本地游戏，足以承载前端 fetch 调用与静态文件托管。
#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <fstream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

struct HttpRequest {
    std::string method;
    std::string path;
    std::string query;
    std::map<std::string, std::string> headers;
    std::string body;
};

struct HttpResponse {
    int status = 200;
    std::string contentType = "text/plain; charset=utf-8";
    std::string body;
    std::map<std::string, std::string> extraHeaders;
};

class HttpServer {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;

    HttpServer() : listenFd(-1) {}
    ~HttpServer() { if (listenFd >= 0) ::close(listenFd); }

    // 路由：method 和 path 完全匹配
    void route(const std::string& method, const std::string& path, Handler h) {
        handlers[method + " " + path] = h;
    }

    void serveStaticDir(const std::string& urlPrefix, const std::string& localDir) {
        staticPrefix = urlPrefix;
        staticDir = localDir;
    }

    bool start(int port) {
        listenFd = ::socket(AF_INET, SOCK_STREAM, 0);
        if (listenFd < 0) return false;
        int yes = 1;
        ::setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(port);
        if (::bind(listenFd, (sockaddr*)&addr, sizeof(addr)) < 0) {
            std::perror("bind");
            return false;
        }
        if (::listen(listenFd, 16) < 0) return false;
        std::printf("Server listening on http://127.0.0.1:%d/\n", port);
        std::fflush(stdout);
        return true;
    }

    void run() {
        while (true) {
            sockaddr_in cli{};
            socklen_t len = sizeof(cli);
            int fd = ::accept(listenFd, (sockaddr*)&cli, &len);
            if (fd < 0) {
                if (errno == EINTR) continue;
                break;
            }
            handleClient(fd);
            ::close(fd);
        }
    }

private:
    int listenFd;
    std::map<std::string, Handler> handlers;
    std::string staticPrefix;
    std::string staticDir;

    void handleClient(int fd) {
        // 读取直到 \r\n\r\n + Content-Length 个字节
        std::string buf;
        char tmp[4096];
        while (true) {
            ssize_t n = ::recv(fd, tmp, sizeof(tmp), 0);
            if (n <= 0) return;
            buf.append(tmp, tmp + n);
            auto p = buf.find("\r\n\r\n");
            if (p != std::string::npos) {
                std::string header = buf.substr(0, p);
                size_t bodyStart = p + 4;
                size_t cl = 0;
                auto clPos = lowerFind(header, "content-length:");
                if (clPos != std::string::npos) {
                    size_t colon = header.find(':', clPos);
                    size_t e = header.find("\r\n", colon);
                    cl = std::strtoul(header.substr(colon + 1, e - colon - 1).c_str(), nullptr, 10);
                }
                while (buf.size() < bodyStart + cl) {
                    ssize_t m = ::recv(fd, tmp, sizeof(tmp), 0);
                    if (m <= 0) break;
                    buf.append(tmp, tmp + m);
                }
                break;
            }
            if (buf.size() > 1 << 20) return;  // 1MB 限制
        }

        HttpRequest req = parseRequest(buf);
        HttpResponse res = dispatch(req);
        sendResponse(fd, res);
    }

    static size_t lowerFind(const std::string& s, const std::string& needle) {
        std::string ls = s, ln = needle;
        for (auto& c : ls) c = (char)std::tolower((unsigned char)c);
        for (auto& c : ln) c = (char)std::tolower((unsigned char)c);
        return ls.find(ln);
    }

    HttpRequest parseRequest(const std::string& raw) {
        HttpRequest req;
        size_t p = raw.find("\r\n\r\n");
        std::string head = raw.substr(0, p);
        req.body = (p == std::string::npos) ? "" : raw.substr(p + 4);

        // 第一行
        size_t eol = head.find("\r\n");
        std::string firstLine = head.substr(0, eol);
        size_t s1 = firstLine.find(' ');
        size_t s2 = firstLine.find(' ', s1 + 1);
        req.method = firstLine.substr(0, s1);
        std::string fullPath = firstLine.substr(s1 + 1, s2 - s1 - 1);
        size_t q = fullPath.find('?');
        if (q != std::string::npos) {
            req.path = fullPath.substr(0, q);
            req.query = fullPath.substr(q + 1);
        } else {
            req.path = fullPath;
        }

        // 头
        size_t pos = eol + 2;
        while (pos < head.size()) {
            size_t e = head.find("\r\n", pos);
            if (e == std::string::npos) e = head.size();
            std::string line = head.substr(pos, e - pos);
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string k = line.substr(0, colon);
                std::string v = line.substr(colon + 1);
                while (!v.empty() && v.front() == ' ') v.erase(v.begin());
                for (auto& c : k) c = (char)std::tolower((unsigned char)c);
                req.headers[k] = v;
            }
            pos = e + 2;
        }
        return req;
    }

    HttpResponse dispatch(const HttpRequest& req) {
        auto it = handlers.find(req.method + " " + req.path);
        if (it != handlers.end()) return it->second(req);

        // 静态文件
        if (!staticDir.empty() && req.method == "GET") {
            std::string rel = (req.path == "/") ? "/index.html" : req.path;
            return serveFile(staticDir + rel);
        }

        HttpResponse r;
        r.status = 404;
        r.body = "Not Found";
        return r;
    }

    HttpResponse serveFile(const std::string& path) {
        HttpResponse r;
        // 防止简单的目录穿越
        if (path.find("..") != std::string::npos) {
            r.status = 400;
            r.body = "Bad path";
            return r;
        }
        std::ifstream f(path, std::ios::binary);
        if (!f) { r.status = 404; r.body = "Not Found: " + path; return r; }
        std::ostringstream ss;
        ss << f.rdbuf();
        r.body = ss.str();
        r.contentType = guessMime(path);
        return r;
    }

    static std::string guessMime(const std::string& path) {
        auto endsWith = [&](const std::string& s) {
            return path.size() >= s.size() && path.compare(path.size() - s.size(), s.size(), s) == 0;
        };
        if (endsWith(".html")) return "text/html; charset=utf-8";
        if (endsWith(".css")) return "text/css; charset=utf-8";
        if (endsWith(".js")) return "application/javascript; charset=utf-8";
        if (endsWith(".json")) return "application/json; charset=utf-8";
        if (endsWith(".svg")) return "image/svg+xml";
        if (endsWith(".png")) return "image/png";
        return "application/octet-stream";
    }

    void sendResponse(int fd, const HttpResponse& r) {
        std::ostringstream os;
        os << "HTTP/1.1 " << r.status << " " << statusText(r.status) << "\r\n";
        os << "Content-Type: " << r.contentType << "\r\n";
        os << "Content-Length: " << r.body.size() << "\r\n";
        os << "Connection: close\r\n";
        os << "Access-Control-Allow-Origin: *\r\n";
        for (auto& h : r.extraHeaders) os << h.first << ": " << h.second << "\r\n";
        os << "\r\n";
        std::string head = os.str();
        ::send(fd, head.data(), head.size(), 0);
        if (!r.body.empty()) ::send(fd, r.body.data(), r.body.size(), 0);
    }

    static const char* statusText(int s) {
        switch (s) {
            case 200: return "OK";
            case 400: return "Bad Request";
            case 404: return "Not Found";
            case 500: return "Internal Server Error";
        }
        return "OK";
    }
};
