#include "httplib.h"
#include "ZumaGame.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <limits.h>
#include <mach-o/dyld.h>

using namespace httplib;

// 将游戏状态加上预览信息
std::string getGameStateWithPreview(const ZumaGame& game) {
    std::string state = game.getStateJson();
    if (state.size() > 1) {
        state.insert(state.size() - 1, ",\"nextBall\":\"" + std::string(1, game.getNextBall()) + "\"");
    }
    return state;
}

// 从可执行文件路径获取public目录路径
std::string getPublicDir() {
    char path[PATH_MAX];
    uint32_t size = sizeof(path);
    if (_NSGetExecutablePath(path, &size) == 0) {
        char resolved[PATH_MAX];
        if (realpath(path, resolved) != nullptr) {
            std::string exePath(resolved);
            size_t pos = exePath.rfind('/');
            if (pos != std::string::npos) {
                return exePath.substr(0, pos) + "/public";
            }
        }
    }
    return "./public";
}

int main() {
    Server svr;

    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    ZumaGame game;

    std::string publicDir = getPublicDir();
    svr.set_mount_point("/", publicDir);

    svr.Get("/api/state", [&](const Request&, Response& res) {
        res.set_content(getGameStateWithPreview(game), "application/json");
    });

    svr.Get("/api/fire", [&](const Request& req, Response& res) {
        int pos = 0;
        if (req.has_param("pos")) {
            pos = std::stoi(req.get_param_value("pos"));
        }
        game.fireBall(pos);
        res.set_content(getGameStateWithPreview(game), "application/json");
    });

    svr.Get("/api/powerup/bomb", [&](const Request& req, Response& res) {
        char color = 'R';
        if (req.has_param("color")) {
            color = req.get_param_value("color")[0];
        }
        game.useBomb(color);
        res.set_content(getGameStateWithPreview(game), "application/json");
    });

    svr.Get("/api/powerup/reverse", [&](const Request&, Response& res) {
        game.useReverse();
        res.set_content(getGameStateWithPreview(game), "application/json");
    });

    svr.Get("/api/powerup/shuffle", [&](const Request&, Response& res) {
        game.useShuffle();
        res.set_content(getGameStateWithPreview(game), "application/json");
    });

    svr.Get("/api/undo", [&](const Request&, Response& res) {
        game.undo();
        res.set_content(getGameStateWithPreview(game), "application/json");
    });

    svr.Get("/api/newgame", [&](const Request&, Response& res) {
        game.initGame();
        res.set_content(getGameStateWithPreview(game), "application/json");
    });

    svr.Options(".*", [&](const Request&, Response& res) {
        res.set_content("", "text/plain");
    });

    std::cout << "=========================================" << std::endl;
    std::cout << "  Zuma Game Server Starting..." << std::endl;
    std::cout << "  Public dir: " << publicDir << std::endl;
    std::cout << "  Open http://localhost:8080 in your browser" << std::endl;
    std::cout << "=========================================" << std::endl;

    svr.listen("0.0.0.0", 8080);
    return 0;
}