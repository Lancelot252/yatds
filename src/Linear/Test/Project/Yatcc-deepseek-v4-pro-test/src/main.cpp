#include "zuma_game.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <vector>

/**
 * 简易 HTTP 服务器 - 祖玛游戏后端
 *
 * 使用 POSIX socket API 实现轻量级 HTTP 服务器，
 * 无需任何第三方依赖，适合教学场景。
 *
 * API 端点：
 *   GET  /           -> 返回 index.html
 *   GET  /style.css  -> 返回 CSS 文件
 *   GET  /app.js     -> 返回 JS 文件
 *   GET  /api/state  -> 返回游戏状态 JSON
 *   POST /api/init   -> 初始化游戏
 *   POST /api/fire   -> 发射彩球 (pos)
 *   POST /api/undo   -> 撤销操作
 *   POST /api/bomb   -> 炸弹道具 (pos)
 *   POST /api/shuffle-> 随机变换道具
 *   POST /api/pushback->轨道回退道具
 *   POST /api/rainbow-> 彩虹球道具 (pos)
 */

ZumaGame game;

// 简易 URL 解码
std::string urlDecode(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); i++) {
        if (s[i] == '%' && i + 2 < s.size()) {
            int c;
            sscanf(s.substr(i + 1, 2).c_str(), "%x", &c);
            result += (char)c;
            i += 2;
        } else if (s[i] == '+') {
            result += ' ';
        } else {
            result += s[i];
        }
    }
    return result;
}

// 解析 POST body
std::string getBody(const std::string& request) {
    size_t pos = request.find("\r\n\r\n");
    if (pos == std::string::npos) return "";
    return request.substr(pos + 4);
}

// 从 request 中查询参数值
std::string getQueryParam(const std::string& request, const std::string& key) {
    // 先在 URL 行中查找 GET 参数
    size_t lineEnd = request.find("\r\n");
    std::string firstLine = request.substr(0, lineEnd);

    size_t qPos = firstLine.find("?");
    if (qPos != std::string::npos) {
        std::string query = firstLine.substr(qPos + 1);
        // 去掉 HTTP 版本部分
        size_t sp = query.find(" ");
        if (sp != std::string::npos) query = query.substr(0, sp);

        size_t kp = query.find(key + "=");
        if (kp != std::string::npos) {
            size_t vs = kp + key.size() + 1;
            size_t ve = query.find("&", vs);
            if (ve == std::string::npos) ve = query.size();
            return urlDecode(query.substr(vs, ve - vs));
        }
    }

    // 再查 POST body
    std::string body = getBody(request);
    size_t kp = body.find(key + "=");
    if (kp != std::string::npos) {
        size_t vs = kp + key.size() + 1;
        size_t ve = body.find("&", vs);
        if (ve == std::string::npos) ve = body.size();
        return urlDecode(body.substr(vs, ve - vs));
    }

    return "";
}

// 读取文件内容
std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f) return "";
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// 获取 MIME 类型
std::string getMimeType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos)  return "text/css";
    if (path.find(".js") != std::string::npos)   return "application/javascript";
    if (path.find(".json") != std::string::npos) return "application/json";
    return "text/plain";
}

// 构建 HTTP 响应
std::string httpResponse(const std::string& body, const std::string& mime = "text/html", int code = 200) {
    std::ostringstream resp;
    resp << "HTTP/1.1 " << code << (code == 200 ? " OK" : " Error") << "\r\n";
    resp << "Content-Type: " << mime << "; charset=utf-8\r\n";
    resp << "Content-Length: " << body.size() << "\r\n";
    resp << "Access-Control-Allow-Origin: *\r\n";
    resp << "Connection: close\r\n";
    resp << "\r\n";
    resp << body;
    return resp.str();
}

// 解析 JSON 请求中的字段
std::string jsonValue(const std::string& json, const std::string& key) {
    // 简单 JSON 解析：查找 "key": value
    size_t start = json.find("\"" + key + "\"");
    if (start == std::string::npos) return "";
    start = json.find(":", start);
    if (start == std::string::npos) return "";
    start++;
    while (start < json.size() && (json[start] == ' ' || json[start] == '\t')) start++;

    if (start >= json.size()) return "";

    std::string val;
    if (json[start] == '"') {
        start++; // skip "
        size_t end = json.find("\"", start);
        if (end == std::string::npos) return "";
        return json.substr(start, end - start);
    } else {
        size_t end = json.find_first_of(",}\n\r \t", start);
        if (end == std::string::npos) end = json.size();
        return json.substr(start, end - start);
    }
}

// 处理 API 请求
std::string handleAPI(const std::string& path, const std::string& all, const std::string& body) {
    // GET /api/state
    if (path.find("/api/state") == 0) {
        return httpResponse(game.getStateJSON(), "application/json");
    }

    // POST 请求处理
    std::string posStr, val;

    if (path.find("/api/init") == 0) {
        val = jsonValue(body, "trackSize");
        int trackSize = val.empty() ? 10 : std::stoi(val);
        val = jsonValue(body, "queueSize");
        int queueSize = val.empty() ? 20 : std::stoi(val);
        val = jsonValue(body, "maxTrack");
        int maxTrack = val.empty() ? 20 : std::stoi(val);
        game.init(trackSize, queueSize, maxTrack);
        return httpResponse(game.getStateJSON(), "application/json");
    }

    if (path.find("/api/fire") == 0) {
        val = jsonValue(body, "pos");
        int pos = val.empty() ? 0 : std::stoi(val);
        game.fire(pos);
        return httpResponse(game.getStateJSON(), "application/json");
    }

    if (path.find("/api/undo") == 0) {
        game.undo();
        return httpResponse(game.getStateJSON(), "application/json");
    }

    if (path.find("/api/bomb") == 0) {
        val = jsonValue(body, "pos");
        int pos = val.empty() ? 0 : std::stoi(val);
        game.bomb(pos);
        return httpResponse(game.getStateJSON(), "application/json");
    }

    if (path.find("/api/shuffle") == 0) {
        game.shuffleNextBall();
        return httpResponse(game.getStateJSON(), "application/json");
    }

    if (path.find("/api/pushback") == 0) {
        game.pushBack();
        return httpResponse(game.getStateJSON(), "application/json");
    }

    if (path.find("/api/rainbow") == 0) {
        val = jsonValue(body, "pos");
        int pos = val.empty() ? 0 : std::stoi(val);
        game.rainbow(pos);
        return httpResponse(game.getStateJSON(), "application/json");
    }

    return httpResponse("{\"error\":\"unknown api\"}", "application/json", 404);
}

// 处理 HTTP 请求
std::string handleRequest(const std::string& request) {
    if (request.empty()) return "";

    size_t lineEnd = request.find("\r\n");
    std::string firstLine = request.substr(0, lineEnd);

    // 解析 method 和 path
    size_t m1 = firstLine.find(" ");
    if (m1 == std::string::npos) return httpResponse("Bad Request", "text/plain", 400);

    std::string method = firstLine.substr(0, m1);
    size_t m2 = firstLine.find(" ", m1 + 1);
    std::string fullPath = firstLine.substr(m1 + 1, m2 - m1 - 1);

    // 去掉 query string 获取路径
    std::string path = fullPath;
    size_t qp = path.find("?");
    if (qp != std::string::npos) path = path.substr(0, qp);

    std::string body = getBody(request);

    std::cout << method << " " << path << std::endl;

    // API 路由
    if (path.find("/api/") == 0) {
        return handleAPI(path, request, body);
    }

    // 静态文件路由
    std::string filePath;
    std::string mime;
    if (path == "/") {
        filePath = "public/index.html";
    } else if (path == "/style.css" || path == "/app.js") {
        filePath = "public" + path;
    } else {
        return httpResponse("Not Found", "text/plain", 404);
    }

    std::string content = readFile(filePath);
    if (content.empty()) {
        return httpResponse("File Not Found: " + filePath, "text/plain", 404);
    }
    return httpResponse(content, getMimeType(filePath));
}

void handleClient(int clientSock) {
    char buffer[8192];
    memset(buffer, 0, sizeof(buffer));

    int bytes = recv(clientSock, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        close(clientSock);
        return;
    }

    std::string response = handleRequest(std::string(buffer, bytes));
    send(clientSock, response.c_str(), response.size(), 0);
    close(clientSock);
}

int main() {
    int port = 8080;
    int serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    int opt = 1;
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(serverSock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Failed to bind port " << port << std::endl;
        close(serverSock);
        return 1;
    }

    if (listen(serverSock, 10) < 0) {
        std::cerr << "Failed to listen" << std::endl;
        close(serverSock);
        return 1;
    }

    std::cout << "🎮 祖玛游戏服务器已启动: http://localhost:" << port << std::endl;
    std::cout << "按 Ctrl+C 停止服务器" << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSock = accept(serverSock, (sockaddr*)&clientAddr, &clientLen);
        if (clientSock < 0) continue;

        std::thread(handleClient, clientSock).detach();
    }

    close(serverSock);
    return 0;
}