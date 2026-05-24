#include "httplib.h"
#include "ZumaGame.hpp"
#include <iostream>
#include <string>

// Helper to replace all occurrences of a string
void replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

int main() {
    httplib::Server svr;
    ZumaGame game;

    svr.Get("/", [](const httplib::Request& req, httplib::Response& res) {
        // Simple HTML file reading logic (could be improved in a real app)
        FILE* f = fopen("index.html", "r");
        if (f) {
            fseek(f, 0, SEEK_END);
            size_t size = ftell(f);
            fseek(f, 0, SEEK_SET);
            std::string content(size, ' ');
            fread(&content[0], 1, size, f);
            fclose(f);
            res.set_content(content, "text/html");
        } else {
            res.set_content("Try running from the directory where index.html is located.", "text/plain");
        }
    });

    svr.Get("/api/state", [&game](const httplib::Request&, httplib::Response& res) {
        std::string json = "{ \"board\": \"" + game.get_board_str() + "\", \"next\": \"" + game.get_next_balls_str() + "\" }";
        res.set_content(json, "application/json");
    });

    svr.Post("/api/insert", [&game](const httplib::Request& req, httplib::Response& res) {
        if (req.has_param("index")) {
            int index = std::stoi(req.get_param_value("index"));
            game.insert_ball(index);
        }
        res.set_redirect("/api/state");
    });

    svr.Post("/api/undo", [&game](const httplib::Request& req, httplib::Response& res) {
        game.undo();
        res.set_redirect("/api/state");
    });

    svr.Post("/api/restart", [&game](const httplib::Request& req, httplib::Response& res) {
        game.init_game();
        res.set_redirect("/api/state");
    });

    std::cout << "Starting Zuma game server at http://localhost:8080..." << std::endl;
    svr.listen("0.0.0.0", 8080);

    return 0;
}
