#include "zuma_game.hpp"
#include <iostream>
#include <sstream>
#include <map>

// 简单的 JSON 响应生成器
class JsonResponse {
public:
    static std::string success(const std::string& message, const std::map<std::string, std::string>& data = {}) {
        std::stringstream ss;
        ss << "{\"success\": true, \"message\": \"" << message << "\"";
        
        for (const auto& [key, value] : data) {
            ss << ", \"" << key << "\": \"" << value << "\"";
        }
        ss << "}";
        return ss.str();
    }
    
    static std::string error(const std::string& message) {
        return "{\"success\": false, \"error\": \"" + message + "\"}";
    }
};

// 全局游戏实例
ZumaGame game;

// 处理 HTTP 请求的函数
std::string handleRequest(const std::string& path, const std::string& method, const std::string& body) {
    if (path == "/api/init" && method == "POST") {
        // 解析 JSON 请求
        // 预期格式: {"track": "R B B G", "queue": "R G B"}
        size_t trackPos = body.find("\"track\":");
        size_t queuePos = body.find("\"queue\":");
        
        if (trackPos == std::string::npos || queuePos == std::string::npos) {
            return JsonResponse::error("Invalid request format");
        }
        
        // 提取 track 值
        size_t trackStart = body.find("\"", trackPos + 8) + 1;
        size_t trackEnd = body.find("\"", trackStart);
        std::string track = body.substr(trackStart, trackEnd - trackStart);
        
        // 提取 queue 值
        size_t queueStart = body.find("\"", queuePos + 8) + 1;
        size_t queueEnd = body.find("\"", queueStart);
        std::string queue = body.substr(queueStart, queueEnd - queueStart);
        
        game.init(track, queue);
        
        std::map<std::string, std::string> data;
        data["track"] = game.getTrackStatus();
        data["queue"] = game.getQueueStatus();
        
        return JsonResponse::success("Game initialized", data);
    }
    
    else if (path == "/api/insert" && method == "POST") {
        // 预期格式: {"position": 3, "color": "R"}
        size_t posPos = body.find("\"position\":");
        size_t colorPos = body.find("\"color\":");
        
        if (posPos == std::string::npos || colorPos == std::string::npos) {
            return JsonResponse::error("Invalid request format");
        }
        
        int position = std::stoi(body.substr(posPos + 11));
        
        size_t colorStart = body.find("\"", colorPos + 8) + 1;
        size_t colorEnd = body.find("\"", colorStart);
        std::string colorStr = body.substr(colorStart, colorEnd - colorStart);
        
        Ball ball(Ball::fromChar(colorStr[0]));
        
        if (game.insertBall(position, ball)) {
            std::map<std::string, std::string> data;
            data["track"] = game.getTrackStatus();
            data["queue"] = game.getQueueStatus();
            data["eliminations"] = std::to_string(game.getEliminations());
            data["isWin"] = game.isWin() ? "true" : "false";
            data["isLose"] = game.isLose() ? "true" : "false";
            
            return JsonResponse::success("Ball inserted", data);
        } else {
            return JsonResponse::error("Invalid position");
        }
    }
    
    else if (path == "/api/status" && method == "GET") {
        std::map<std::string, std::string> data;
        data["track"] = game.getTrackStatus();
        data["queue"] = game.getQueueStatus();
        data["trackSize"] = std::to_string(game.getTrackSize());
        data["queueSize"] = std::to_string(game.getQueueSize());
        data["eliminations"] = std::to_string(game.getEliminations());
        data["isWin"] = game.isWin() ? "true" : "false";
        data["isLose"] = game.isLose() ? "true" : "false";
        
        return JsonResponse::success("Current game status", data);
    }
    
    else if (path == "/api/undo" && method == "POST") {
        if (game.undo()) {
            std::map<std::string, std::string> data;
            data["track"] = game.getTrackStatus();
            data["queue"] = game.getQueueStatus();
            
            return JsonResponse::success("Action undone", data);
        } else {
            return JsonResponse::error("Cannot undo");
        }
    }
    
    else if (path == "/api/reset" && method == "POST") {
        game.reset();
        return JsonResponse::success("Game reset");
    }
    
    else if (path == "/" && method == "GET") {
        return "<!DOCTYPE html><html><head><title>Zuma Game</title></head><body><p>Zuma Game Server is running</p></body></html>";
    }
    
    return JsonResponse::error("Endpoint not found");
}

// 简单的 HTTP 服务器（仅演示用，真实版本需要使用 httplib 或其他库）
int main() {
    std::cout << "Zuma Game Server starting..." << std::endl;
    std::cout << "Usage: " << std::endl;
    std::cout << "POST /api/init with body: {\"track\": \"R B B G\", \"queue\": \"R G B\"}" << std::endl;
    std::cout << "POST /api/insert with body: {\"position\": 2, \"color\": \"R\"}" << std::endl;
    std::cout << "GET /api/status" << std::endl;
    std::cout << "POST /api/undo" << std::endl;
    std::cout << "POST /api/reset" << std::endl;
    
    // 测试代码
    std::cout << "\n=== Testing Zuma Game ===" << std::endl;
    
    game.init("R B B B R", "R G B");
    std::cout << "Initial track: " << game.getTrackStatus() << std::endl;
    std::cout << "Initial queue: " << game.getQueueStatus() << std::endl;
    
    // 测试插入
    Ball testBall(BallColor::BLUE);
    game.insertBall(2, testBall);
    std::cout << "After insert at 2: " << game.getTrackStatus() << std::endl;
    std::cout << "Eliminations: " << game.getEliminations() << std::endl;
    
    // 测试撤销
    game.undo();
    std::cout << "After undo: " << game.getTrackStatus() << std::endl;
    
    std::cout << "\n=== Test completed ===" << std::endl;
    
    return 0;
}
