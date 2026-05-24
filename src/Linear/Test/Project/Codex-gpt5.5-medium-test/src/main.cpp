#include <algorithm>
#include <array>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <deque>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace {

constexpr int kDefaultPort = 8080;
constexpr int kMaxTurns = 45;
constexpr int kWinScore = 120;

std::string trim(const std::string &text) {
    const auto begin = text.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) return "";
    const auto end = text.find_last_not_of(" \t\r\n");
    return text.substr(begin, end - begin + 1);
}

std::string urlDecode(const std::string &value) {
    std::string result;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] == '%' && i + 2 < value.size()) {
            const std::string hex = value.substr(i + 1, 2);
            result.push_back(static_cast<char>(std::strtol(hex.c_str(), nullptr, 16)));
            i += 2;
        } else if (value[i] == '+') {
            result.push_back(' ');
        } else {
            result.push_back(value[i]);
        }
    }
    return result;
}

std::map<std::string, std::string> parseQuery(const std::string &query) {
    std::map<std::string, std::string> values;
    std::stringstream stream(query);
    std::string pair;
    while (std::getline(stream, pair, '&')) {
        const auto sep = pair.find('=');
        if (sep == std::string::npos) {
            values[urlDecode(pair)] = "";
        } else {
            values[urlDecode(pair.substr(0, sep))] = urlDecode(pair.substr(sep + 1));
        }
    }
    return values;
}

std::string jsonEscape(const std::string &value) {
    std::string out;
    for (char ch : value) {
        switch (ch) {
            case '\\': out += "\\\\"; break;
            case '"': out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out.push_back(ch); break;
        }
    }
    return out;
}

struct Snapshot {
    std::list<char> track;
    std::deque<char> launcher;
    int score = 0;
    int turns = 0;
    int bombs = 0;
    int shuffles = 0;
    std::string message;
    bool gameOver = false;
    bool won = false;
};

class ZumaGame {
public:
    ZumaGame() {
        reset();
    }

    void reset() {
        track_ = {'R', 'G', 'G', 'B', 'Y', 'R', 'P', 'P', 'B', 'Y', 'M', 'R'};
        launcher_.clear();
        const std::string script = "GBRYPMPBRGYMYRGBPBRYMGPRYBMGPRYBGRPMY";
        for (char color : script) launcher_.push_back(color);
        undo_ = std::stack<Snapshot>();
        score_ = 0;
        turns_ = 0;
        bombs_ = 2;
        shuffles_ = 1;
        gameOver_ = false;
        won_ = false;
        message_ = "新游戏开始。插入待发射彩球，制造三个或以上同色连线。";
    }

    void insertBall(size_t position) {
        ensureActive();
        if (launcher_.empty()) throw std::runtime_error("待发射队列已空，无法继续发射。");
        if (position > track_.size()) throw std::runtime_error("插入位置越界。");
        save();

        const char ball = launcher_.front();
        launcher_.pop_front();
        auto it = track_.begin();
        std::advance(it, static_cast<long>(position));
        track_.insert(it, ball);
        ++turns_;

        const int removed = resolveMatches();
        std::stringstream msg;
        msg << "发射 " << ball << " 到位置 " << position << "。";
        if (removed > 0) msg << " 连锁消除了 " << removed << " 个彩球。";
        else msg << " 暂未形成消除。";
        message_ = msg.str();
        updateResult();
    }

    void useBomb(size_t position) {
        ensureActive();
        if (bombs_ <= 0) throw std::runtime_error("炸弹已经用完。");
        if (track_.empty()) throw std::runtime_error("轨道为空，不需要使用炸弹。");
        if (position >= track_.size()) throw std::runtime_error("炸弹位置越界。");
        save();

        auto it = track_.begin();
        std::advance(it, static_cast<long>(position));
        const char target = *it;
        int removed = 0;
        for (auto cur = track_.begin(); cur != track_.end();) {
            if (*cur == target) {
                cur = track_.erase(cur);
                ++removed;
            } else {
                ++cur;
            }
        }
        --bombs_;
        ++turns_;
        score_ += removed * 8;
        removed += resolveMatches();

        std::stringstream msg;
        msg << "炸弹清除了所有 " << target << " 色彩球，本回合共移除 " << removed << " 个。";
        message_ = msg.str();
        updateResult();
    }

    void useShuffle() {
        ensureActive();
        if (shuffles_ <= 0) throw std::runtime_error("重排道具已经用完。");
        if (launcher_.size() < 2) throw std::runtime_error("待发射队列不足，无法重排。");
        save();

        std::rotate(launcher_.begin(), launcher_.begin() + 3 % launcher_.size(), launcher_.end());
        --shuffles_;
        message_ = "重排道具已使用：待发射队列循环前移，给当前局面换一种出球顺序。";
    }

    void undo() {
        if (undo_.empty()) {
            message_ = "没有可以撤销的操作。";
            return;
        }
        restore(undo_.top());
        undo_.pop();
        message_ = "已撤销上一步操作。";
    }

    std::string stateJson() const {
        std::stringstream out;
        out << "{";
        out << "\"track\":" << arrayJson(track_) << ",";
        out << "\"launcher\":" << arrayJson(launcher_) << ",";
        out << "\"score\":" << score_ << ",";
        out << "\"turns\":" << turns_ << ",";
        out << "\"maxTurns\":" << kMaxTurns << ",";
        out << "\"bombs\":" << bombs_ << ",";
        out << "\"shuffles\":" << shuffles_ << ",";
        out << "\"undo\":" << (undo_.empty() ? "false" : "true") << ",";
        out << "\"gameOver\":" << (gameOver_ ? "true" : "false") << ",";
        out << "\"won\":" << (won_ ? "true" : "false") << ",";
        out << "\"message\":\"" << jsonEscape(message_) << "\"";
        out << "}";
        return out.str();
    }

private:
    template <typename Container>
    std::string arrayJson(const Container &items) const {
        std::stringstream out;
        out << "[";
        bool first = true;
        for (char item : items) {
            if (!first) out << ",";
            first = false;
            out << "\"" << item << "\"";
        }
        out << "]";
        return out.str();
    }

    void ensureActive() const {
        if (gameOver_) throw std::runtime_error("游戏已经结束，请重新开始。");
    }

    void save() {
        undo_.push({track_, launcher_, score_, turns_, bombs_, shuffles_, message_, gameOver_, won_});
    }

    void restore(const Snapshot &snapshot) {
        track_ = snapshot.track;
        launcher_ = snapshot.launcher;
        score_ = snapshot.score;
        turns_ = snapshot.turns;
        bombs_ = snapshot.bombs;
        shuffles_ = snapshot.shuffles;
        gameOver_ = snapshot.gameOver;
        won_ = snapshot.won;
    }

    int resolveMatches() {
        int totalRemoved = 0;
        bool changed = true;
        while (changed) {
            changed = false;
            auto runStart = track_.begin();
            while (runStart != track_.end()) {
                auto runEnd = runStart;
                while (runEnd != track_.end() && *runEnd == *runStart) ++runEnd;
                const int count = static_cast<int>(std::distance(runStart, runEnd));
                if (count >= 3) {
                    const char color = *runStart;
                    track_.erase(runStart, runEnd);
                    totalRemoved += count;
                    score_ += count * 10 + (count - 3) * 5;
                    if (count >= 5 && launcher_.size() < 50) {
                        launcher_.push_back(color);
                    }
                    changed = true;
                    break;
                }
                runStart = runEnd;
            }
        }
        return totalRemoved;
    }

    void updateResult() {
        if (track_.empty()) {
            gameOver_ = true;
            won_ = true;
            score_ += 50;
            message_ += " 轨道清空，胜利！";
        } else if (score_ >= kWinScore) {
            gameOver_ = true;
            won_ = true;
            message_ += " 分数达到目标，胜利！";
        } else if (turns_ >= kMaxTurns || launcher_.empty()) {
            gameOver_ = true;
            won_ = false;
            message_ += " 回合或弹药耗尽，游戏失败。";
        }
    }

    std::list<char> track_;
    std::deque<char> launcher_;
    std::stack<Snapshot> undo_;
    int score_ = 0;
    int turns_ = 0;
    int bombs_ = 0;
    int shuffles_ = 0;
    std::string message_;
    bool gameOver_ = false;
    bool won_ = false;
};

class Server {
public:
    explicit Server(int port) : port_(port) {}

    void run() {
        int serverFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverFd < 0) throw std::runtime_error("无法创建 socket。");

        int opt = 1;
        setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in address{};
        address.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
        address.sin_port = htons(static_cast<uint16_t>(port_));

        if (bind(serverFd, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0) {
            close(serverFd);
            throw std::runtime_error("端口绑定失败，请换一个端口。");
        }
        if (listen(serverFd, 16) < 0) {
            close(serverFd);
            throw std::runtime_error("监听端口失败。");
        }

        std::cout << "Zuma server running at http://127.0.0.1:" << port_ << "\n";
        while (true) {
            const int client = accept(serverFd, nullptr, nullptr);
            if (client < 0) continue;
            handle(client);
            close(client);
        }
    }

private:
    void handle(int client) {
        std::array<char, 8192> buffer{};
        const ssize_t bytes = recv(client, buffer.data(), buffer.size() - 1, 0);
        if (bytes <= 0) return;

        std::stringstream request(std::string(buffer.data(), static_cast<size_t>(bytes)));
        std::string method;
        std::string target;
        request >> method >> target;

        try {
            if (method != "GET" && method != "POST") {
                sendResponse(client, 405, "application/json", "{\"error\":\"method not allowed\"}");
                return;
            }
            route(client, target);
        } catch (const std::exception &error) {
            std::string body = std::string("{\"ok\":false,\"error\":\"") + jsonEscape(error.what()) +
                               "\",\"state\":" + game_.stateJson() + "}";
            sendResponse(client, 400, "application/json", body);
        }
    }

    void route(int client, const std::string &target) {
        const auto sep = target.find('?');
        const std::string path = sep == std::string::npos ? target : target.substr(0, sep);
        const std::string query = sep == std::string::npos ? "" : target.substr(sep + 1);
        const auto params = parseQuery(query);

        if (path == "/" || path == "/index.html") {
            sendFile(client, "public/index.html", "text/html; charset=utf-8");
        } else if (path == "/favicon.ico") {
            sendResponse(client, 204, "image/x-icon", "");
        } else if (path == "/style.css") {
            sendFile(client, "public/style.css", "text/css; charset=utf-8");
        } else if (path == "/app.js") {
            sendFile(client, "public/app.js", "application/javascript; charset=utf-8");
        } else if (path == "/api/state") {
            sendJson(client, okJson());
        } else if (path == "/api/reset") {
            game_.reset();
            sendJson(client, okJson());
        } else if (path == "/api/insert") {
            game_.insertBall(numberParam(params, "pos"));
            sendJson(client, okJson());
        } else if (path == "/api/bomb") {
            game_.useBomb(numberParam(params, "pos"));
            sendJson(client, okJson());
        } else if (path == "/api/shuffle") {
            game_.useShuffle();
            sendJson(client, okJson());
        } else if (path == "/api/undo") {
            game_.undo();
            sendJson(client, okJson());
        } else {
            sendResponse(client, 404, "text/plain; charset=utf-8", "Not found");
        }
    }

    size_t numberParam(const std::map<std::string, std::string> &params, const std::string &key) const {
        const auto it = params.find(key);
        if (it == params.end() || trim(it->second).empty()) throw std::runtime_error("缺少参数：" + key);
        return static_cast<size_t>(std::stoul(it->second));
    }

    std::string okJson() const {
        return std::string("{\"ok\":true,\"state\":") + game_.stateJson() + "}";
    }

    void sendJson(int client, const std::string &body) {
        sendResponse(client, 200, "application/json; charset=utf-8", body);
    }

    void sendFile(int client, const std::string &path, const std::string &type) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            sendResponse(client, 404, "text/plain; charset=utf-8", "File not found");
            return;
        }
        std::ostringstream body;
        body << file.rdbuf();
        sendResponse(client, 200, type, body.str());
    }

    void sendResponse(int client, int code, const std::string &type, const std::string &body) {
        const std::string status = code == 200 ? "OK" : code == 204 ? "No Content" : code == 400 ? "Bad Request" :
                                   code == 404 ? "Not Found" : code == 405 ? "Method Not Allowed" :
                                   "Internal Server Error";
        std::stringstream response;
        response << "HTTP/1.1 " << code << " " << status << "\r\n";
        response << "Content-Type: " << type << "\r\n";
        response << "Content-Length: " << body.size() << "\r\n";
        response << "Connection: close\r\n";
        response << "Access-Control-Allow-Origin: *\r\n\r\n";
        response << body;
        const std::string data = response.str();
        send(client, data.c_str(), data.size(), 0);
    }

    int port_;
    ZumaGame game_;
};

}  // namespace

int main(int argc, char **argv) {
    std::signal(SIGPIPE, SIG_IGN);
    int port = kDefaultPort;
    if (argc >= 2) port = std::atoi(argv[1]);
    try {
        Server(port).run();
    } catch (const std::exception &error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
    return 0;
}
