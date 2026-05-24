#include "zuma_game.hpp"
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <signal.h>
#include <fstream>

// 全局游戏实例
ZumaGame game;

// 简单的HTTP服务器实现
// 支持GET和POST请求，用于前后端通信

// 解析HTTP请求
struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::string contentType;
};

// 构建HTTP响应
std::string buildHttpResponse(const std::string& statusCode,
                               const std::string& contentType,
                               const std::string& body) {
    std::ostringstream oss;
    oss << "HTTP/1.1 " << statusCode << "\r\n";
    oss << "Content-Type: " << contentType << "\r\n";
    oss << "Content-Length: " << body.size() << "\r\n";
    oss << "Access-Control-Allow-Origin: *\r\n";
    oss << "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
    oss << "Access-Control-Allow-Headers: Content-Type\r\n";
    oss << "Connection: close\r\n";
    oss << "\r\n";
    oss << body;
    return oss.str();
}

// 解析HTTP请求
HttpRequest parseHttpRequest(const std::string& raw) {
    HttpRequest req;
    std::istringstream iss(raw);

    // 解析请求行
    std::string line;
    if (std::getline(iss, line)) {
        // 去除行末的\r
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream lineIss(line);
        lineIss >> req.method >> req.path;
    }

    // 解析头部
    while (std::getline(iss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) break; // 空行表示头部结束
        if (line.find("Content-Type:") != std::string::npos) {
            req.contentType = line.substr(line.find(":") + 2);
        }
    }

    // 读取剩余内容作为body
    std::string remaining;
    while (std::getline(iss, remaining)) {
        req.body += remaining;
    }

    return req;
}

// 从JSON body中提取整数值
int extractIntFromJson(const std::string& json, const std::string& key) {
    std::string searchStr = "\"" + key + "\"";
    size_t pos = json.find(searchStr);
    if (pos == std::string::npos) return -1;
    pos = json.find(":", pos);
    if (pos == std::string::npos) return -1;
    pos++; // 跳过冒号

    // 跳过空格
    while (pos < json.size() && json[pos] == ' ') pos++;

    // 读取数字
    std::string numStr;
    while (pos < json.size() && (json[pos] == '-' || (json[pos] >= '0' && json[pos] <= '9'))) {
        numStr += json[pos];
        pos++;
    }
    if (numStr.empty()) return -1;
    return std::stoi(numStr);
}

// 从JSON body中提取字符串值
std::string extractStringFromJson(const std::string& json, const std::string& key) {
    std::string searchStr = "\"" + key + "\"";
    size_t pos = json.find(searchStr);
    if (pos == std::string::npos) return "";
    pos = json.find(":", pos);
    if (pos == std::string::npos) return "";
    pos++; // 跳过冒号

    // 跳过空格
    while (pos < json.size() && json[pos] == ' ') pos++;

    // 跳过引号
    if (pos < json.size() && json[pos] == '"') pos++;

    // 读取字符串
    std::string result;
    while (pos < json.size() && json[pos] != '"') {
        result += json[pos];
        pos++;
    }
    return result;
}

// 处理API请求
std::string handleApiRequest(const HttpRequest& req) {
    // OPTIONS请求（CORS预检）
    if (req.method == "OPTIONS") {
        return buildHttpResponse("200 OK", "text/plain", "");
    }

    // GET /api/state - 获取游戏状态
    if (req.method == "GET" && req.path == "/api/state") {
        return buildHttpResponse("200 OK", "application/json", game.getStateJson());
    }

    // POST /api/init - 初始化游戏
    if (req.method == "POST" && req.path == "/api/init") {
        int trackSize = extractIntFromJson(req.body, "trackSize");
        int queueSize = extractIntFromJson(req.body, "queueSize");
        int maxTrack = extractIntFromJson(req.body, "maxTrack");

        if (trackSize <= 0) trackSize = 8;
        if (queueSize <= 0) queueSize = 10;
        if (maxTrack <= 0) maxTrack = 15;

        game.init(trackSize, queueSize, maxTrack);
        return buildHttpResponse("200 OK", "application/json", game.getStateJson());
    }

    // POST /api/fire - 发射彩球
    if (req.method == "POST" && req.path == "/api/fire") {
        int pos = extractIntFromJson(req.body, "pos");
        if (pos < 0) {
            return buildHttpResponse("400 Bad Request", "application/json",
                                     "{\"error\": \"Invalid position\"}");
        }
        EliminationResult result = game.fire(pos);

        std::ostringstream oss;
        oss << "{";
        oss << "\"eliminated\": " << (result.eliminated ? "true" : "false") << ",";
        oss << "\"eliminatedCount\": " << result.eliminatedCount << ",";
        oss << "\"chainCount\": " << result.chainCount << ",";
        oss << "\"state\": " << game.getStateJson();
        oss << "}";
        return buildHttpResponse("200 OK", "application/json", oss.str());
    }

    // POST /api/undo - 撤销操作
    if (req.method == "POST" && req.path == "/api/undo") {
        bool success = game.undo();
        return buildHttpResponse("200 OK", "application/json",
                                 "{\"success\": " + std::string(success ? "true" : "false") +
                                 ", \"state\": " + game.getStateJson() + "}");
    }

    // POST /api/item - 使用道具
    if (req.method == "POST" && req.path == "/api/item") {
        int typeInt = extractIntFromJson(req.body, "type");
        int pos = extractIntFromJson(req.body, "pos");
        std::string colorStr = extractStringFromJson(req.body, "color");

        ItemType type = static_cast<ItemType>(typeInt);
        BallColor color = charToColor(colorStr.empty() ? 'R' : colorStr[0]);

        bool success = game.useItem(type, pos, color);
        return buildHttpResponse("200 OK", "application/json",
                                 "{\"success\": " + std::string(success ? "true" : "false") +
                                 ", \"state\": " + game.getStateJson() + "}");
    }

    // 未知API路径
    return buildHttpResponse("404 Not Found", "application/json",
                             "{\"error\": \"Unknown API endpoint\"}");
}

// 读取静态文件
std::string readStaticFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) return "";
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

// 根据文件扩展名确定Content-Type
std::string getContentType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css") != std::string::npos) return "text/css";
    if (path.find(".js") != std::string::npos) return "application/javascript";
    if (path.find(".png") != std::string::npos) return "image/png";
    if (path.find(".jpg") != std::string::npos) return "image/jpeg";
    if (path.find(".ico") != std::string::npos) return "image/x-icon";
    return "text/plain";
}

// 处理静态文件请求
std::string handleStaticRequest(const HttpRequest& req, const std::string& publicDir) {
    std::string filepath;

    if (req.path == "/" || req.path == "/index.html") {
        filepath = publicDir + "/index.html";
    } else {
        filepath = publicDir + req.path;
    }

    std::string content = readStaticFile(filepath);
    if (content.empty()) {
        return buildHttpResponse("404 Not Found", "text/html", "<h1>404 Not Found</h1>");
    }

    return buildHttpResponse("200 OK", getContentType(filepath), content);
}

// 处理单个客户端连接
void handleClient(int clientFd, const std::string& publicDir) {
    char buffer[4096] = {0};
    ssize_t bytesRead = recv(clientFd, buffer, sizeof(buffer) - 1, 0);

    if (bytesRead <= 0) {
        close(clientFd);
        return;
    }

    std::string rawRequest(buffer, bytesRead);
    HttpRequest req = parseHttpRequest(rawRequest);

    std::string response;

    // API请求
    if (req.path.find("/api/") != std::string::npos) {
        response = handleApiRequest(req);
    } else {
        // 静态文件请求
        response = handleStaticRequest(req, publicDir);
    }

    send(clientFd, response.c_str(), response.size(), 0);
    close(clientFd);
}

int main(int argc, char* argv[]) {
    int port = 8080;
    std::string publicDir = "public";

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--port" && i + 1 < argc) {
            port = std::stoi(argv[i + 1]);
        }
        if (std::string(argv[i]) == "--public" && i + 1 < argc) {
            publicDir = argv[i + 1];
        }
    }

    // 创建TCP socket
    int serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // 设置socket选项，允许地址重用
    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定地址和端口
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(serverFd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Failed to bind to port " << port << std::endl;
        close(serverFd);
        return 1;
    }

    // 开始监听
    if (listen(serverFd, 10) < 0) {
        std::cerr << "Failed to listen" << std::endl;
        close(serverFd);
        return 1;
    }

    std::cout << "Zuma Game Server running on http://localhost:" << port << std::endl;
    std::cout << "Serving static files from: " << publicDir << std::endl;

    // 初始化游戏
    game.init(8, 10, 15);

    // 主循环：接受客户端连接
    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(serverFd, (struct sockaddr*)&clientAddr, &clientLen);

        if (clientFd < 0) {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }

        // 在新线程中处理客户端请求
        std::thread clientThread(handleClient, clientFd, publicDir);
        clientThread.detach();
    }

    close(serverFd);
    return 0;
}