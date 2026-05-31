#!/bin/bash

# 祖玛游戏 - 构建和运行脚本

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$PROJECT_DIR"

echo "========================================="
echo "  祖玛游戏 - Zuma Game"
echo "========================================="

# 检查 httplib.h 是否存在
if [ ! -f "httplib.h" ]; then
    echo "[1/3] Downloading httplib.h..."
    curl -sL https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h -o httplib.h
    echo "      Done!"
else
    echo "[1/3] httplib.h already exists."
fi

# 构建
echo "[2/3] Building server..."
c++ -std=c++11 -O2 -Wall -Wextra -pthread \
    server.cpp \
    -o zuma_server \
    -l pthread 2>/dev/null || \
c++ -std=c++11 -O2 -Wall -pthread \
    server.cpp \
    -o zuma_server

echo "      Build complete!"

# 运行
echo "[3/3] Starting server..."
echo ""
echo "========================================="
echo "  Open http://localhost:8080 in your browser"
echo "========================================="
echo ""

./zuma_server