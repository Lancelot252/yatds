#include "zuma_game.hpp"

#include <arpa/inet.h>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

namespace {

struct Request {
    std::string method;
    std::string path;
    std::map<std::string, std::string> query;
};

std::string statusText(int statusCode) {
    switch (statusCode) {
    case 200:
        return "OK";
    case 204:
        return "No Content";
    case 400:
        return "Bad Request";
    case 404:
        return "Not Found";
    case 405:
        return "Method Not Allowed";
    default:
        return "Internal Server Error";
    }
}

std::string urlDecode(const std::string& value) {
    std::string result;
    for (std::size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2 < value.size()) {
            const std::string hex = value.substr(i + 1, 2);
            char* end = nullptr;
            const long decoded = std::strtol(hex.c_str(), &end, 16);
            if (end != nullptr && *end == '\0') {
                result.push_back(static_cast<char>(decoded));
                i += 2;
            }
        } else if (value[i] == '+') {
            result.push_back(' ');
        } else {
            result.push_back(value[i]);
        }
    }
    return result;
}

std::map<std::string, std::string> parseQuery(const std::string& queryString) {
    std::map<std::string, std::string> query;
    std::size_t start = 0;

    while (start <= queryString.size()) {
        const std::size_t amp = queryString.find('&', start);
        const std::string part = queryString.substr(
            start,
            amp == std::string::npos ? std::string::npos : amp - start
        );
        if (!part.empty()) {
            const std::size_t equals = part.find('=');
            const std::string key = urlDecode(part.substr(0, equals));
            const std::string value = equals == std::string::npos
                ? ""
                : urlDecode(part.substr(equals + 1));
            query[key] = value;
        }
        if (amp == std::string::npos) {
            break;
        }
        start = amp + 1;
    }

    return query;
}

Request parseRequest(const std::string& raw) {
    std::istringstream input(raw);
    std::string line;
    std::getline(input, line);
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    std::istringstream firstLine(line);
    std::string target;
    std::string version;
    Request request;
    firstLine >> request.method >> target >> version;
    if (request.method.empty() || target.empty()) {
        throw std::runtime_error("invalid HTTP request");
    }

    const std::size_t question = target.find('?');
    request.path = question == std::string::npos ? target : target.substr(0, question);
    if (question != std::string::npos) {
        request.query = parseQuery(target.substr(question + 1));
    }

    return request;
}

std::string queryValue(const Request& request, const std::string& key, const std::string& fallback = "") {
    const auto it = request.query.find(key);
    if (it == request.query.end()) {
        return fallback;
    }
    return it->second;
}

bool parseSize(const std::string& value, std::size_t& result) {
    try {
        std::size_t used = 0;
        const unsigned long parsed = std::stoul(value, &used);
        if (used != value.size()) {
            return false;
        }
        result = static_cast<std::size_t>(parsed);
        return true;
    } catch (...) {
        return false;
    }
}

std::string loadTextFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return "";
    }
    std::ostringstream out;
    out << file.rdbuf();
    return out.str();
}

std::string response(int statusCode, const std::string& contentType, const std::string& body) {
    std::ostringstream out;
    out << "HTTP/1.1 " << statusCode << " " << statusText(statusCode) << "\r\n";
    out << "Content-Type: " << contentType << "\r\n";
    out << "Content-Length: " << body.size() << "\r\n";
    out << "Cache-Control: no-store\r\n";
    out << "Access-Control-Allow-Origin: *\r\n";
    out << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    out << "Access-Control-Allow-Headers: Content-Type\r\n";
    out << "Connection: close\r\n\r\n";
    out << body;
    return out.str();
}

void sendAll(int client, const std::string& data) {
    std::size_t sent = 0;
    while (sent < data.size()) {
        const ssize_t n = send(client, data.data() + sent, data.size() - sent, 0);
        if (n <= 0) {
            return;
        }
        sent += static_cast<std::size_t>(n);
    }
}

std::string handleApi(ZumaGame& game, const Request& request) {
    if (request.method == "OPTIONS") {
        return response(204, "text/plain; charset=utf-8", "");
    }

    if (request.path == "/api/state" && request.method == "GET") {
        return response(200, "application/json; charset=utf-8", game.stateJson());
    }

    if (request.path == "/api/reset" && request.method == "POST") {
        game.reset();
        return response(200, "application/json; charset=utf-8", game.stateJson());
    }

    if (request.path == "/api/insert" && request.method == "POST") {
        std::size_t position = 0;
        if (!parseSize(queryValue(request, "pos"), position)) {
            return response(400, "application/json; charset=utf-8", "{\"error\":\"missing or invalid pos\"}");
        }
        game.insertBall(position);
        return response(200, "application/json; charset=utf-8", game.stateJson());
    }

    if (request.path == "/api/undo" && request.method == "POST") {
        game.undo();
        return response(200, "application/json; charset=utf-8", game.stateJson());
    }

    if (request.path == "/api/rotate" && request.method == "POST") {
        game.rotateLauncher();
        return response(200, "application/json; charset=utf-8", game.stateJson());
    }

    if (request.path == "/api/powerup" && request.method == "POST") {
        std::size_t position = 0;
        if (!parseSize(queryValue(request, "pos"), position)) {
            return response(400, "application/json; charset=utf-8", "{\"error\":\"missing or invalid pos\"}");
        }
        game.usePowerup(queryValue(request, "type"), position);
        return response(200, "application/json; charset=utf-8", game.stateJson());
    }

    return response(404, "application/json; charset=utf-8", "{\"error\":\"api not found\"}");
}

std::string handleRequest(ZumaGame& game, const Request& request) {
    if (request.path.rfind("/api/", 0) == 0) {
        return handleApi(game, request);
    }

    if (request.method != "GET") {
        return response(405, "text/plain; charset=utf-8", "Method Not Allowed");
    }

    if (request.path == "/favicon.ico") {
        return response(204, "image/x-icon", "");
    }

    if (request.path == "/" || request.path == "/index.html") {
        const std::string html = loadTextFile("web/index.html");
        if (html.empty()) {
            return response(404, "text/plain; charset=utf-8", "web/index.html not found. Run the server from the project root.");
        }
        return response(200, "text/html; charset=utf-8", html);
    }

    return response(404, "text/plain; charset=utf-8", "Not Found");
}

int createServerSocket(int port) {
    const int server = socket(AF_INET, SOCK_STREAM, 0);
    if (server < 0) {
        throw std::runtime_error("socket failed");
    }

    int enabled = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled));

    sockaddr_in address {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(static_cast<uint16_t>(port));

    if (bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0) {
        close(server);
        throw std::runtime_error("bind failed");
    }

    if (listen(server, 16) < 0) {
        close(server);
        throw std::runtime_error("listen failed");
    }

    return server;
}

} // namespace

int main(int argc, char* argv[]) {
    std::signal(SIGPIPE, SIG_IGN);

    int port = 8080;
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            std::cerr << "Invalid port: " << argv[1] << "\n";
            return 1;
        }
    }

    try {
        ZumaGame game;
        const int server = createServerSocket(port);
        std::cout << "Zuma server running at http://localhost:" << port << "\n";
        std::cout << "Press Ctrl+C to stop.\n";

        while (true) {
            sockaddr_in clientAddress {};
            socklen_t clientLength = sizeof(clientAddress);
            const int client = accept(server, reinterpret_cast<sockaddr*>(&clientAddress), &clientLength);
            if (client < 0) {
                continue;
            }

            char buffer[8192];
            const ssize_t readCount = recv(client, buffer, sizeof(buffer) - 1, 0);
            if (readCount <= 0) {
                close(client);
                continue;
            }
            buffer[readCount] = '\0';

            try {
                const Request request = parseRequest(buffer);
                const std::string reply = handleRequest(game, request);
                sendAll(client, reply);
            } catch (const std::exception&) {
                sendAll(client, response(400, "text/plain; charset=utf-8", "Bad Request"));
            }

            close(client);
        }
    } catch (const std::exception& error) {
        std::cerr << "Server error: " << error.what() << "\n";
        return 1;
    }
}
