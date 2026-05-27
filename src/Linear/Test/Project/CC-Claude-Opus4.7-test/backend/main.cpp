// 祖玛游戏后端入口：装配游戏对象，注册路由，启动 HTTP 服务。
#include "Game.hpp"
#include "HttpServer.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

// 简单 JSON 字段查找：从形如 {"key":123,"k2":"v"} 的字符串中找 key 的数字。
// 失败返回 fallback。仅用于解析前端发来的非常简单的 JSON。
static long jsonGetInt(const std::string& body, const std::string& key, long fallback) {
    std::string needle = "\"" + key + "\"";
    auto p = body.find(needle);
    if (p == std::string::npos) return fallback;
    p = body.find(':', p);
    if (p == std::string::npos) return fallback;
    ++p;
    while (p < body.size() && (body[p] == ' ' || body[p] == '\t')) ++p;
    if (p >= body.size()) return fallback;
    char* endp = nullptr;
    long v = std::strtol(body.c_str() + p, &endp, 10);
    if (endp == body.c_str() + p) return fallback;
    return v;
}

static HttpResponse jsonOk(const std::string& body) {
    HttpResponse r;
    r.status = 200;
    r.contentType = "application/json; charset=utf-8";
    r.body = body;
    return r;
}

static std::string actionJson(const ActionResult& a, const Game& g) {
    std::ostringstream os;
    os << "{\"success\":" << (a.success ? "true" : "false")
       << ",\"message\":\"" << a.message << "\""
       << ",\"eliminated\":" << a.eliminatedTotal
       << ",\"chain\":" << a.chainCount
       << ",\"state\":" << g.toJson() << "}";
    return os.str();
}

int main(int argc, char** argv) {
    int port = 8080;
    std::string staticDir = "../frontend";
    for (int i = 1; i + 1 < argc; ++i) {
        if (std::strcmp(argv[i], "--port") == 0) port = std::atoi(argv[i + 1]);
        else if (std::strcmp(argv[i], "--static") == 0) staticDir = argv[i + 1];
    }

    Game game;
    game.reset(0);

    HttpServer srv;
    srv.serveStaticDir("/", staticDir);

    srv.route("GET", "/api/state", [&](const HttpRequest&) {
        return jsonOk(game.toJson());
    });

    srv.route("POST", "/api/init", [&](const HttpRequest& req) {
        long seed = jsonGetInt(req.body, "seed", 0);
        long colors = jsonGetInt(req.body, "numColors", -1);
        long pendingLen = jsonGetInt(req.body, "queueLength", -1);
        long initLen = jsonGetInt(req.body, "initialTrackLength", -1);
        if (colors >= 3 && colors <= 8) game.numColors = (int)colors;
        if (pendingLen > 0 && pendingLen <= 100) game.queueLength = (int)pendingLen;
        if (initLen >= 0 && initLen <= 25) game.initialTrackLength = (int)initLen;
        game.reset((unsigned)seed);
        return jsonOk(game.toJson());
    });

    srv.route("POST", "/api/shoot", [&](const HttpRequest& req) {
        long pos = jsonGetInt(req.body, "position", 0);
        if (pos < 0) pos = 0;
        auto r = game.shoot((size_t)pos);
        return jsonOk(actionJson(r, game));
    });

    srv.route("POST", "/api/undo", [&](const HttpRequest&) {
        auto r = game.undo();
        return jsonOk(actionJson(r, game));
    });

    srv.route("POST", "/api/bomb", [&](const HttpRequest& req) {
        long pos = jsonGetInt(req.body, "position", 0);
        if (pos < 0) pos = 0;
        auto r = game.useBomb((size_t)pos);
        return jsonOk(actionJson(r, game));
    });

    srv.route("POST", "/api/recolor", [&](const HttpRequest& req) {
        long pos = jsonGetInt(req.body, "position", 0);
        long color = jsonGetInt(req.body, "color", 1);
        if (pos < 0) pos = 0;
        auto r = game.useRecolor((size_t)pos, (int)color);
        return jsonOk(actionJson(r, game));
    });

    srv.route("POST", "/api/skip", [&](const HttpRequest&) {
        auto r = game.useSkip();
        return jsonOk(actionJson(r, game));
    });

    if (!srv.start(port)) {
        std::cerr << "Failed to start server on port " << port << std::endl;
        return 1;
    }
    srv.run();
    return 0;
}
