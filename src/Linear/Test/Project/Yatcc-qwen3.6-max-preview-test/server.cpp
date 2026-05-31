/**
 * @file server.cpp
 * @brief 祖玛游戏 HTTP 后端服务器
 *
 * 使用 cpp-httplib 提供 RESTful API，前端通过 JSON 与后端通信。
 *
 * API 端点：
 *   POST /api/init       - 初始化游戏
 *   POST /api/insert     - 插入彩球
 *   POST /api/undo       - 撤销操作
 *   GET  /api/status     - 获取游戏状态
 *   GET  /               - 返回 index.html
 */

#include "httplib.h"
#include "ZumaGame.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>
#include <random>
#include <string>
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

/// @brief 获取可执行文件所在目录
static std::string getExeDir() {
    char path[PATH_MAX];
#ifdef __APPLE__
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        char* dir = dirname(path);
        return std::string(dir);
    }
#else
    ssize_t len = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (len != -1) {
        path[len] = '\0';
        char* dir = dirname(path);
        return std::string(dir);
    }
#endif
    return ".";
}

// 全局游戏实例（带互斥锁保护）
static ZumaGame game;
static std::mutex gameMutex;

/// @brief 简单 JSON 解析：提取字符串值
static std::string jsonGetString(const std::string& json, const std::string& key) {
    std::string searchKey = "\"" + key + "\"";
    size_t pos = json.find(searchKey);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos);
    if (pos == std::string::npos) return "";

    // 跳过空白
    while (++pos < json.size() && (json[pos] == ' ' || json[pos] == '\t' || json[pos] == '\n'));

    if (pos >= json.size()) return "";

    if (json[pos] == '"') {
        size_t end = json.find('"', pos + 1);
        if (end == std::string::npos) return "";
        return json.substr(pos + 1, end - pos - 1);
    }

    // 数字
    size_t end = pos;
    while (end < json.size() && json[end] != ',' && json[end] != '}' && json[end] != ' ') {
        end++;
    }
    return json.substr(pos, end - pos);
}

/// @brief 简单 JSON 解析：提取整数值
static int jsonGetInt(const std::string& json, const std::string& key) {
    std::string val = jsonGetString(json, key);
    if (val.empty()) return -1;
    try {
        return std::stoi(val);
    } catch (...) {
        return -1;
    }
}

/// @brief 生成随机初始轨道
static std::string generateRandomTrack(int length, const std::string& colors) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, (int)colors.size() - 1);

    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; i++) {
        result += colors[dis(gen)];
    }
    return result;
}

/// @brief 生成随机待发射队列
static std::string generateRandomQueue(int length, const std::string& colors) {
    return generateRandomTrack(length, colors);
}

/// @brief 读取文件内容
static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char** argv) {
    int port = 8080;
    if (argc > 1) {
        port = std::atoi(argv[1]);
        if (port <= 0 || port > 65535) port = 8080;
    }

    std::string exeDir = getExeDir();

    httplib::Server svr;

    // ============================================================
    // 静态文件服务
    // ============================================================
    svr.Get("/", [&exeDir](const httplib::Request&, httplib::Response& res) {
        std::string html = readFile(exeDir + "/index.html");
        if (html.empty()) {
            res.status = 404;
            res.set_content("index.html not found", "text/plain");
            return;
        }
        res.set_content(html, "text/html; charset=utf-8");
    });

    // ============================================================
    // POST /api/init - 初始化游戏
    // ============================================================
    svr.Post("/api/init", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(gameMutex);

        std::string track = jsonGetString(req.body, "track");
        std::string queue = jsonGetString(req.body, "queue");
        std::string mode = jsonGetString(req.body, "mode");

        // 如果 mode 为 "random"，自动生成
        if (mode == "random") {
            std::string colors = "RGBY";
            int trackLen = jsonGetInt(req.body, "trackLength");
            int queueLen = jsonGetInt(req.body, "queueLength");
            if (trackLen <= 0) trackLen = 10;
            if (queueLen <= 0) queueLen = 15;
            track = generateRandomTrack(trackLen, colors);
            queue = generateRandomQueue(queueLen, colors);
        }

        if (track.empty()) {
            track = "RRBBGGYYRR";
        }
        if (queue.empty()) {
            queue = "RBGYRBGYRBGY";
        }

        game.initGame(track, queue);

        res.set_content(game.toJson(), "application/json");
    });

    // ============================================================
    // POST /api/insert - 插入彩球
    // ============================================================
    svr.Post("/api/insert", [](const httplib::Request& req, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(gameMutex);

        int position = jsonGetInt(req.body, "position");
        if (position < 0) {
            res.status = 400;
            res.set_content("{\"error\":\"Invalid position\"}", "application/json");
            return;
        }

        int eliminated = game.insertBall(position);
        if (eliminated == -1) {
            res.status = 400;
            res.set_content("{\"error\":\"Insert failed\"}", "application/json");
            return;
        }

        std::string response = game.toJson();
        // 添加消除信息
        response = response.substr(0, response.size() - 1); // 去掉最后的 }
        response += ",\"eliminated\":" + std::to_string(eliminated) + "}";

        res.set_content(response, "application/json");
    });

    // ============================================================
    // POST /api/undo - 撤销操作
    // ============================================================
    svr.Post("/api/undo", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(gameMutex);

        bool success = game.undo();
        if (!success) {
            res.status = 400;
            res.set_content("{\"error\":\"Undo failed\"}", "application/json");
            return;
        }

        res.set_content(game.toJson(), "application/json");
    });

    // ============================================================
    // GET /api/status - 获取游戏状态
    // ============================================================
    svr.Get("/api/status", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(gameMutex);
        res.set_content(game.toJson(), "application/json");
    });

    std::cout << "========================================" << std::endl;
    std::cout << "  祖玛游戏服务器启动中..." << std::endl;
    std::cout << "  地址: http://localhost:" << port << std::endl;
    std::cout << "========================================" << std::endl;

    svr.listen("0.0.0.0", port);

    return 0;
}
