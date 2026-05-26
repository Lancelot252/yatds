#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <map>
#include <vector>
#include <cstring>
#include <cstdlib>
#include "ZumaGame.hpp"

// Simple HTTP server implementation
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef int socklen_t;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

class HttpServer {
private:
    SOCKET serverSocket;
    int port;
    bool running;
    std::map<std::string, std::string> routes;
    std::mutex gameMutex;
    ZumaGame game;

public:
    HttpServer(int p = 8080) : port(p), serverSocket(INVALID_SOCKET), running(false) {}

    ~HttpServer() {
        stop();
    }

    bool start() {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return false;
        }
#endif

        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed" << std::endl;
            return false;
        }

        // 允许地址重用
        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(port);

        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed" << std::endl;
            closesocket(serverSocket);
            return false;
        }

        if (listen(serverSocket, 10) == SOCKET_ERROR) {
            std::cerr << "Listen failed" << std::endl;
            closesocket(serverSocket);
            return false;
        }

        running = true;
        std::cout << "Server started on port " << port << std::endl;
        std::cout << "Open http://localhost:" << port << " in your browser" << std::endl;

        return true;
    }

    void run() {
        while (running) {
            sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            
            SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientLen);
            if (clientSocket == INVALID_SOCKET) {
                continue;
            }

            // 处理客户端请求
            handleClient(clientSocket);
            closesocket(clientSocket);
        }
    }

    void stop() {
        running = false;
        if (serverSocket != INVALID_SOCKET) {
            closesocket(serverSocket);
            serverSocket = INVALID_SOCKET;
        }
#ifdef _WIN32
        WSACleanup();
#endif
    }

private:
    void handleClient(SOCKET clientSocket) {
        char buffer[4096];
        int received = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) return;

        buffer[received] = '\0';
        std::string request(buffer);

        // 解析请求
        std::string method, path;
        std::istringstream requestStream(request);
        requestStream >> method >> path;

        std::string response;

        // 路由处理
        if (path == "/" || path == "/index.html") {
            response = getHtmlResponse();
        } else if (path == "/api/state") {
            std::lock_guard<std::mutex> lock(gameMutex);
            response = getJsonResponse(game.getGameStateJson());
        } else if (path.find("/api/launch/") == 0) {
            // 发射彩球 /api/launch/{position}
            int pos = extractPosition(path);
            std::lock_guard<std::mutex> lock(gameMutex);
            bool success = game.launchBall(pos);
            response = getJsonResponse("{\"success\":" + std::string(success ? "true" : "false") + 
                                       ",\"state\":" + game.getGameStateJson() + "}");
        } else if (path == "/api/undo") {
            std::lock_guard<std::mutex> lock(gameMutex);
            bool success = game.undo();
            response = getJsonResponse("{\"success\":" + std::string(success ? "true" : "false") + 
                                       ",\"state\":" + game.getGameStateJson() + "}");
        } else if (path == "/api/reset") {
            std::lock_guard<std::mutex> lock(gameMutex);
            game.initGame();
            response = getJsonResponse("{\"success\":true,\"state\":" + game.getGameStateJson() + "}");
        } else if (path.find("/api/bomb/") == 0) {
            // 使用炸弹 /api/bomb/{position}
            int pos = extractPosition(path);
            std::lock_guard<std::mutex> lock(gameMutex);
            bool success = game.useBomb(pos);
            response = getJsonResponse("{\"success\":" + std::string(success ? "true" : "false") + 
                                       ",\"state\":" + game.getGameStateJson() + "}");
        } else if (path.find("/api/refresh") == 0) {
            // 刷新队列
            std::lock_guard<std::mutex> lock(gameMutex);
            bool success = game.useRefreshQueue();
            response = getJsonResponse("{\"success\":" + std::string(success ? "true" : "false") + 
                                       ",\"state\":" + game.getGameStateJson() + "}");
        } else {
            response = getNotFoundResponse();
        }

        send(clientSocket, response.c_str(), response.length(), 0);
    }

    int extractPosition(const std::string& path) {
        size_t lastSlash = path.find_last_of('/');
        if (lastSlash == std::string::npos) return 0;
        try {
            return std::stoi(path.substr(lastSlash + 1));
        } catch (...) {
            return 0;
        }
    }

    std::string getHtmlResponse() {
        std::string html = 
R"HTML(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Zuma Game</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #1a1a2e 0%, #16213e 50%, #0f3460 100%);
            min-height: 100vh;
            display: flex;
            flex-direction: column;
            align-items: center;
            padding: 20px;
            color: white;
        }
        
        h1 {
            font-size: 2.5em;
            margin-bottom: 10px;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.5);
            background: linear-gradient(45deg, #ff6b6b, #feca57, #48dbfb, #ff9ff3);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
            background-clip: text;
        }
        
        .game-container {
            background: rgba(255,255,255,0.1);
            border-radius: 20px;
            padding: 30px;
            margin: 20px;
            backdrop-filter: blur(10px);
            box-shadow: 0 8px 32px rgba(0,0,0,0.3);
            max-width: 900px;
            width: 100%;
        }
        
        .info-panel {
            display: flex;
            justify-content: space-around;
            margin-bottom: 20px;
            padding: 15px;
            background: rgba(0,0,0,0.2);
            border-radius: 10px;
        }
        
        .info-item {
            text-align: center;
        }
        
        .info-label {
            font-size: 0.9em;
            color: #aaa;
            margin-bottom: 5px;
        }
        
        .info-value {
            font-size: 1.5em;
            font-weight: bold;
            color: #feca57;
        }
        
        .track-container {
            background: linear-gradient(180deg, #2d3436 0%, #000000 100%);
            border-radius: 50px;
            padding: 20px;
            margin: 20px 0;
            min-height: 100px;
            display: flex;
            align-items: center;
            justify-content: flex-start;
            overflow-x: auto;
            position: relative;
            box-shadow: inset 0 4px 8px rgba(0,0,0,0.5);
        }
        
        .ball {
            width: 50px;
            height: 50px;
            border-radius: 50%;
            margin: 0 3px;
            cursor: pointer;
            transition: all 0.3s ease;
            position: relative;
            box-shadow: 
                inset -5px -5px 10px rgba(0,0,0,0.3),
                inset 5px 5px 10px rgba(255,255,255,0.3),
                0 4px 8px rgba(0,0,0,0.3);
        }
        
        .ball:hover {
            transform: scale(1.15);
            z-index: 10;
        }
        
        .ball.red { background: radial-gradient(circle at 30% 30%, #ff6b6b, #c0392b); }
        .ball.blue { background: radial-gradient(circle at 30% 30%, #48dbfb, #2980b9); }
        .ball.green { background: radial-gradient(circle at 30% 30%, #1dd1a1, #27ae60); }
        .ball.yellow { background: radial-gradient(circle at 30% 30%, #feca57, #f39c12); }
        .ball.purple { background: radial-gradient(circle at 30% 30%, #ff9ff3, #9b59b6); }
        
        .ball::after {
            content: '';
            position: absolute;
            top: 15%;
            left: 20%;
            width: 25%;
            height: 25%;
            background: radial-gradient(circle, rgba(255,255,255,0.8), transparent);
            border-radius: 50%;
        }
        
        .insert-marker {
            width: 4px;
            height: 60px;
            background: rgba(255,255,255,0.5);
            margin: 0 2px;
            border-radius: 2px;
            cursor: pointer;
            transition: all 0.2s;
        }
        
        .insert-marker:hover {
            background: #feca57;
            transform: scaleY(1.2);
        }
        
        .queue-container {
            display: flex;
            justify-content: center;
            align-items: center;
            margin: 20px 0;
            padding: 15px;
            background: rgba(0,0,0,0.2);
            border-radius: 10px;
        }
        
        .queue-label {
            margin-right: 15px;
            font-size: 1.1em;
        }
        
        .queue-balls {
            display: flex;
            gap: 10px;
        }
        
        .queue-ball {
            width: 40px;
            height: 40px;
            border-radius: 50%;
            opacity: 0.7;
            box-shadow: 
                inset -3px -3px 6px rgba(0,0,0,0.3),
                inset 3px 3px 6px rgba(255,255,255,0.3);
        }
        
        .queue-ball:first-child {
            opacity: 1;
            transform: scale(1.1);
            border: 3px solid #feca57;
        }
        
        .controls {
            display: flex;
            justify-content: center;
            gap: 15px;
            margin-top: 20px;
        }
        
        button {
            padding: 12px 24px;
            font-size: 1em;
            border: none;
            border-radius: 25px;
            cursor: pointer;
            transition: all 0.3s;
            font-weight: bold;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        
        .btn-primary {
            background: linear-gradient(45deg, #667eea, #764ba2);
            color: white;
        }
        
        .btn-secondary {
            background: linear-gradient(45deg, #f093fb, #f5576c);
            color: white;
        }
        
        .btn-tool {
            background: linear-gradient(45deg, #4facfe, #00f2fe);
            color: white;
        }
        
        button:hover {
            transform: translateY(-2px);
            box-shadow: 0 8px 20px rgba(0,0,0,0.3);
        }
        
        button:disabled {
            opacity: 0.5;
            cursor: not-allowed;
            transform: none;
        }
        
        .message {
            position: fixed;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            background: rgba(0,0,0,0.9);
            padding: 40px 60px;
            border-radius: 20px;
            text-align: center;
            z-index: 1000;
            display: none;
        }
        
        .message.show {
            display: block;
            animation: popIn 0.3s ease;
        }
        
        @keyframes popIn {
            from { transform: translate(-50%, -50%) scale(0.5); opacity: 0; }
            to { transform: translate(-50%, -50%) scale(1); opacity: 1; }
        }
        
        .message h2 {
            font-size: 2.5em;
            margin-bottom: 20px;
        }
        
        .message.win h2 { color: #1dd1a1; }
        .message.lose h2 { color: #ff6b6b; }
        
        .instructions {
            margin-top: 20px;
            padding: 15px;
            background: rgba(255,255,255,0.05);
            border-radius: 10px;
            font-size: 0.9em;
            line-height: 1.6;
        }
        
        .instructions h3 {
            margin-bottom: 10px;
            color: #feca57;
        }
        
        .loading {
            display: inline-block;
            width: 20px;
            height: 20px;
            border: 3px solid rgba(255,255,255,0.3);
            border-radius: 50%;
            border-top-color: #feca57;
            animation: spin 1s ease-in-out infinite;
        }
        
        @keyframes spin {
            to { transform: rotate(360deg); }
        }
    </style>
</head>
<body>
    <h1>Zuma Game</h1>
    
    <div class="game-container">
        <div class="info-panel">
            <div class="info-item">
                <div class="info-label">Score</div>
                <div class="info-value" id="score">0</div>
            </div>
            <div class="info-item">
                <div class="info-label">Moves</div>
                <div class="info-value" id="moves">0/50</div>
            </div>
            <div class="info-item">
                <div class="info-label">Track Length</div>
                <div class="info-value" id="trackLength">0</div>
            </div>
        </div>
        
        <div class="track-container" id="track">
        </div>
        
        <div class="queue-container">
            <div class="queue-label">Launch Queue:</div>
            <div class="queue-balls" id="queue">
            </div>
        </div>
        
        <div class="controls">
            <button class="btn-secondary" onclick="undo()">Undo</button>
            <button class="btn-primary" onclick="resetGame()">New Game</button>
        </div>
        
        <div class="controls">
            <button class="btn-tool" onclick="useBomb()">Bomb</button>
            <button class="btn-tool" onclick="useRefresh()">Refresh Queue</button>
        </div>
        
        <div class="instructions">
            <h3>How to Play</h3>
            <p>Click the <b>insert markers</b> on the track to launch balls. When 3 or more balls of the same color connect, they will be eliminated.</p>
            <p>After elimination, if the balls on both sides have the same color, chain elimination will occur!</p>
            <p><b>Goal:</b> Clear the track or reach 1000 points to win!</p>
            <p><b>Tools:</b> Bomb can eliminate balls at and around the target position. Refresh can regenerate the launch queue.</p>
        </div>
    </div>
    
    <div class="message" id="message">
        <h2 id="messageTitle"></h2>
        <p id="messageText"></p>
        <button class="btn-primary" onclick="resetGame()">Play Again</button>
    </div>

    <script>
        let gameState = null;
        
        async function fetchState() {
            try {
                const response = await fetch('/api/state');
                gameState = await response.json();
                updateUI();
            } catch (error) {
                console.error('Error fetching state:', error);
            }
        }
        
        function updateUI() {
            if (!gameState) return;
            
            document.getElementById('score').textContent = gameState.score;
            document.getElementById('moves').textContent = gameState.moves + '/' + gameState.maxMoves;
            document.getElementById('trackLength').textContent = gameState.trackLength;
            
            const trackDiv = document.getElementById('track');
            trackDiv.innerHTML = '';
            
            const startMarker = document.createElement('div');
            startMarker.className = 'insert-marker';
            startMarker.onclick = () => launchBall(0);
            startMarker.title = 'Insert at beginning';
            trackDiv.appendChild(startMarker);
            
            gameState.track.forEach((color, index) => {
                const ball = document.createElement('div');
                ball.className = 'ball ' + color;
                trackDiv.appendChild(ball);
                
                const marker = document.createElement('div');
                marker.className = 'insert-marker';
                marker.onclick = () => launchBall(index + 1);
                marker.title = 'Insert at position ' + (index + 1);
                trackDiv.appendChild(marker);
            });
            
            const queueDiv = document.getElementById('queue');
            queueDiv.innerHTML = '';
            gameState.queue.forEach((color, index) => {
                const ball = document.createElement('div');
                ball.className = 'queue-ball ball ' + color;
                queueDiv.appendChild(ball);
            });
            
            if (gameState.gameOver) {
                showMessage(gameState.win ? 'win' : 'lose', 
                    gameState.win ? 'Congratulations! You Win!' : 'Game Over',
                    gameState.win ? 'Your score: ' + gameState.score : 'Try again!');
            }
        }
        
        async function launchBall(position) {
            try {
                const response = await fetch('/api/launch/' + position);
                const result = await response.json();
                if (result.success) {
                    gameState = result.state;
                    updateUI();
                }
            } catch (error) {
                console.error('Error launching ball:', error);
            }
        }
        
        async function undo() {
            try {
                const response = await fetch('/api/undo');
                const result = await response.json();
                if (result.success) {
                    gameState = result.state;
                    updateUI();
                }
            } catch (error) {
                console.error('Error undoing:', error);
            }
        }
        
        async function resetGame() {
            try {
                const response = await fetch('/api/reset');
                const result = await response.json();
                gameState = result.state;
                hideMessage();
                updateUI();
            } catch (error) {
                console.error('Error resetting game:', error);
            }
        }
        
        async function useBomb() {
            const position = Math.floor(gameState.trackLength / 2);
            try {
                const response = await fetch('/api/bomb/' + position);
                const result = await response.json();
                if (result.success) {
                    gameState = result.state;
                    updateUI();
                }
            } catch (error) {
                console.error('Error using bomb:', error);
            }
        }
        
        async function useRefresh() {
            try {
                const response = await fetch('/api/refresh');
                const result = await response.json();
                if (result.success) {
                    gameState = result.state;
                    updateUI();
                }
            } catch (error) {
                console.error('Error refreshing queue:', error);
            }
        }
        
        function showMessage(type, title, text) {
            const messageDiv = document.getElementById('message');
            messageDiv.className = 'message show ' + type;
            document.getElementById('messageTitle').textContent = title;
            document.getElementById('messageText').textContent = text;
        }
        
        function hideMessage() {
            document.getElementById('message').className = 'message';
        }
        
        fetchState();
        setInterval(fetchState, 2000);
    </script>
</body>
</html>)HTML";

        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html; charset=utf-8\r\n"
               "Content-Length: " + std::to_string(html.length()) + "\r\n"
               "Connection: close\r\n\r\n" + html;
    }

    std::string getJsonResponse(const std::string& json) {
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: application/json; charset=utf-8\r\n"
               "Content-Length: " + std::to_string(json.length()) + "\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Connection: close\r\n\r\n" + json;
    }

    std::string getNotFoundResponse() {
        std::string body = "{\"error\":\"Not Found\"}";
        return "HTTP/1.1 404 Not Found\r\n"
               "Content-Type: application/json\r\n"
               "Content-Length: " + std::to_string(body.length()) + "\r\n"
               "Connection: close\r\n\r\n" + body;
    }
};

int main(int argc, char* argv[]) {
    int port = 8080;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }

    HttpServer server(port);
    
    if (!server.start()) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    server.run();
    
    return 0;
}
