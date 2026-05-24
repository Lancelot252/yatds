#include <algorithm>
#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <netinet/in.h>
#include <optional>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

namespace {

struct Snapshot {
  std::string track;
  std::string queue;
  int score = 0;
  int bombs = 0;
};

struct GameState {
  std::list<char> track;
  std::queue<char> queue;
  std::vector<Snapshot> undo;
  int score = 0;
  int bombs = 2;
  bool win = false;
  bool lose = false;
};

constexpr int kPort = 8080;
const std::string kColors = "RGBY";

std::string read_file(const std::string &path) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    return "";
  }
  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

std::string queue_to_string(std::queue<char> q) {
  std::string out;
  while (!q.empty()) {
    out.push_back(q.front());
    q.pop();
  }
  return out;
}

std::string track_to_string(const std::list<char> &track) {
  std::string out;
  out.reserve(track.size());
  for (char c : track) {
    out.push_back(c);
  }
  return out;
}

std::optional<int> json_int(const std::string &body, const std::string &key) {
  auto pos = body.find("\"" + key + "\"");
  if (pos == std::string::npos) {
    return std::nullopt;
  }
  auto colon = body.find(':', pos);
  if (colon == std::string::npos) {
    return std::nullopt;
  }
  size_t start = body.find_first_of("-0123456789", colon + 1);
  if (start == std::string::npos) {
    return std::nullopt;
  }
  size_t end = body.find_first_not_of("0123456789", start);
  return std::stoi(body.substr(start, end - start));
}

std::optional<std::string> json_string(const std::string &body, const std::string &key) {
  auto pos = body.find("\"" + key + "\"");
  if (pos == std::string::npos) {
    return std::nullopt;
  }
  auto colon = body.find(':', pos);
  if (colon == std::string::npos) {
    return std::nullopt;
  }
  auto quote = body.find('"', colon + 1);
  if (quote == std::string::npos) {
    return std::nullopt;
  }
  auto end = body.find('"', quote + 1);
  if (end == std::string::npos) {
    return std::nullopt;
  }
  return body.substr(quote + 1, end - quote - 1);
}

std::string status_text(const GameState &game) {
  if (game.win) {
    return "Win";
  }
  if (game.lose) {
    return "Lose";
  }
  return "Playing";
}

void update_status(GameState &game) {
  game.win = game.track.empty();
  game.lose = (!game.win && game.queue.empty());
}

void push_undo(GameState &game) {
  game.undo.push_back({track_to_string(game.track), queue_to_string(game.queue), game.score, game.bombs});
  if (game.undo.size() > 50) {
    game.undo.erase(game.undo.begin());
  }
}

void reset_game(GameState &game, const std::optional<std::string> &track_seed,
                const std::optional<std::string> &queue_seed) {
  game.track.clear();
  game.queue = std::queue<char>();
  game.undo.clear();
  game.score = 0;
  game.bombs = 2;
  game.win = false;
  game.lose = false;

  std::string track = track_seed.value_or("");
  std::string queue = queue_seed.value_or("");

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> dist(0, static_cast<int>(kColors.size() - 1));

  if (track.empty()) {
    for (int i = 0; i < 8; ++i) {
      track.push_back(kColors[dist(rng)]);
    }
  }

  if (queue.empty()) {
    for (int i = 0; i < 14; ++i) {
      queue.push_back(kColors[dist(rng)]);
    }
  }

  for (char c : track) {
    if (kColors.find(c) != std::string::npos) {
      game.track.push_back(c);
    }
  }
  for (char c : queue) {
    if (kColors.find(c) != std::string::npos) {
      game.queue.push(c);
    }
  }

  update_status(game);
}

std::list<char>::iterator iter_at(std::list<char> &track, int pos) {
  auto it = track.begin();
  int idx = 0;
  while (it != track.end() && idx < pos) {
    ++it;
    ++idx;
  }
  return it;
}

bool chain_eliminate(GameState &game, std::list<char>::iterator it) {
  if (game.track.empty()) {
    return false;
  }

  bool removed_any = false;
  while (true) {
    if (it == game.track.end()) {
      break;
    }
    char color = *it;

    auto left = it;
    auto right = it;

    while (left != game.track.begin()) {
      auto prev = left;
      --prev;
      if (*prev == color) {
        left = prev;
      } else {
        break;
      }
    }

    auto next = right;
    ++next;
    while (next != game.track.end() && *next == color) {
      right = next;
      ++next;
    }

    int count = 1;
    for (auto cursor = left; cursor != next; ++cursor) {
      ++count;
    }
    count -= 1;

    if (count >= 3) {
      removed_any = true;
      game.score += count * 10;
      auto after = next;
      auto before = left;
      if (before != game.track.begin()) {
        --before;
      } else {
        before = game.track.end();
      }
      game.track.erase(left, next);

      if (before != game.track.end() && after != game.track.end() && *before == *after) {
        it = after;
        continue;
      }
      if (after != game.track.end()) {
        it = after;
        continue;
      }
      break;
    }
    break;
  }

  update_status(game);
  return removed_any;
}

bool shoot_ball(GameState &game, int pos) {
  if (game.queue.empty() || game.win || game.lose) {
    return false;
  }

  push_undo(game);
  char ball = game.queue.front();
  game.queue.pop();

  if (pos < 0) {
    pos = 0;
  }
  if (pos > static_cast<int>(game.track.size())) {
    pos = static_cast<int>(game.track.size());
  }

  auto it = iter_at(game.track, pos);
  auto inserted = game.track.insert(it, ball);
  chain_eliminate(game, inserted);
  return true;
}

bool use_bomb(GameState &game, int pos) {
  if (game.bombs <= 0 || game.track.empty() || game.win || game.lose) {
    return false;
  }
  if (pos < 0 || pos >= static_cast<int>(game.track.size())) {
    return false;
  }

  push_undo(game);
  auto it = iter_at(game.track, pos);
  it = game.track.erase(it);
  game.bombs -= 1;
  if (it != game.track.end()) {
    chain_eliminate(game, it);
  }
  update_status(game);
  return true;
}

bool undo(GameState &game) {
  if (game.undo.empty()) {
    return false;
  }
  Snapshot snap = game.undo.back();
  game.undo.pop_back();

  game.track.clear();
  for (char c : snap.track) {
    game.track.push_back(c);
  }

  game.queue = std::queue<char>();
  for (char c : snap.queue) {
    game.queue.push(c);
  }

  game.score = snap.score;
  game.bombs = snap.bombs;
  update_status(game);
  return true;
}

std::string game_json(const GameState &game) {
  std::ostringstream out;
  out << "{\"track\":\"" << track_to_string(game.track) << "\",";
  out << "\"queue\":\"" << queue_to_string(game.queue) << "\",";
  out << "\"next\":\"" << (game.queue.empty() ? "" : std::string(1, game.queue.front())) << "\",";
  out << "\"score\":" << game.score << ",";
  out << "\"bombs\":" << game.bombs << ",";
  out << "\"status\":\"" << status_text(game) << "\"}";
  return out.str();
}

struct HttpRequest {
  std::string method;
  std::string path;
  std::string body;
};

HttpRequest parse_request(const std::string &raw) {
  std::istringstream stream(raw);
  HttpRequest req;
  stream >> req.method >> req.path;
  auto body_pos = raw.find("\r\n\r\n");
  if (body_pos != std::string::npos) {
    req.body = raw.substr(body_pos + 4);
  }
  return req;
}

std::string http_response(const std::string &body, const std::string &content_type = "text/plain",
                          int status = 200) {
  std::ostringstream out;
  out << "HTTP/1.1 " << status << " OK\r\n";
  out << "Content-Type: " << content_type << "\r\n";
  out << "Content-Length: " << body.size() << "\r\n";
  out << "Connection: close\r\n\r\n";
  out << body;
  return out.str();
}

std::string serve_static(const std::string &root, const std::string &path) {
  std::string file_path = root + path;
  if (path == "/") {
    file_path = root + "/index.html";
  }
  std::string body = read_file(file_path);
  if (body.empty()) {
    return http_response("Not found", "text/plain", 404);
  }

  std::string content_type = "text/plain";
  if (file_path.find(".html") != std::string::npos) {
    content_type = "text/html";
  } else if (file_path.find(".css") != std::string::npos) {
    content_type = "text/css";
  } else if (file_path.find(".js") != std::string::npos) {
    content_type = "application/javascript";
  }

  return http_response(body, content_type, 200);
}

bool read_full_request(int client_fd, std::string &request) {
  char buffer[4096];
  int received = 0;
  request.clear();

  while (true) {
    ssize_t count = recv(client_fd, buffer, sizeof(buffer), 0);
    if (count <= 0) {
      break;
    }
    request.append(buffer, buffer + count);
    received += static_cast<int>(count);
    if (request.find("\r\n\r\n") != std::string::npos) {
      auto header_end = request.find("\r\n\r\n");
      auto header = request.substr(0, header_end);
      auto length_pos = header.find("Content-Length:");
      if (length_pos == std::string::npos) {
        break;
      }
      auto line_end = header.find("\r\n", length_pos);
      auto length_str = header.substr(length_pos + 15, line_end - length_pos - 15);
      int content_length = std::stoi(length_str);
      int body_have = static_cast<int>(request.size() - header_end - 4);
      if (body_have >= content_length) {
        break;
      }
    }
  }

  return !request.empty();
}

void handle_client(int client_fd, GameState &game, const std::string &static_root) {
  std::string raw;
  if (!read_full_request(client_fd, raw)) {
    return;
  }
  auto req = parse_request(raw);
  std::string response;

  if (req.path.rfind("/api/", 0) == 0) {
    if (req.path == "/api/state") {
      response = http_response(game_json(game), "application/json");
    } else if (req.path == "/api/reset") {
      auto track = json_string(req.body, "track");
      auto queue = json_string(req.body, "queue");
      reset_game(game, track, queue);
      response = http_response(game_json(game), "application/json");
    } else if (req.path == "/api/shoot") {
      auto pos = json_int(req.body, "pos");
      if (!pos.has_value()) {
        response = http_response("Bad request", "text/plain", 400);
      } else {
        shoot_ball(game, pos.value());
        response = http_response(game_json(game), "application/json");
      }
    } else if (req.path == "/api/undo") {
      undo(game);
      response = http_response(game_json(game), "application/json");
    } else if (req.path == "/api/bomb") {
      auto pos = json_int(req.body, "pos");
      if (!pos.has_value()) {
        response = http_response("Bad request", "text/plain", 400);
      } else {
        use_bomb(game, pos.value());
        response = http_response(game_json(game), "application/json");
      }
    } else {
      response = http_response("Not found", "text/plain", 404);
    }
  } else {
    response = serve_static(static_root, req.path);
  }

  send(client_fd, response.c_str(), response.size(), 0);
}

}  // namespace

int main() {
  std::signal(SIGPIPE, SIG_IGN);

  GameState game;
  reset_game(game, std::nullopt, std::nullopt);

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "socket() failed: " << std::strerror(errno) << "\n";
    return 1;
  }

  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(kPort);

  if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
    std::cerr << "bind() failed: " << std::strerror(errno) << "\n";
    close(server_fd);
    return 1;
  }

  if (listen(server_fd, 16) < 0) {
    std::cerr << "listen() failed: " << std::strerror(errno) << "\n";
    close(server_fd);
    return 1;
  }

  std::cout << "Zuma server running at http://localhost:" << kPort << "\n";
  std::string static_root = std::string("./public");

  while (true) {
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, reinterpret_cast<sockaddr *>(&client_addr), &client_len);
    if (client_fd < 0) {
      continue;
    }
    handle_client(client_fd, game, static_root);
    close(client_fd);
  }

  close(server_fd);
  return 0;
}
