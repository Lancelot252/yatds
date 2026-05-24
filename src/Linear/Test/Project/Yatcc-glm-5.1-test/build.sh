#!/bin/bash

# 祖玛游戏构建脚本

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
SRC_DIR="$PROJECT_DIR/src"
PUBLIC_DIR="$PROJECT_DIR/public"

echo "=== 构建祖玛游戏服务器 ==="

# 创建构建目录
mkdir -p "$BUILD_DIR"

# 编译
echo "编译中..."
g++ -std=c++17 -O2 -Wall \
    "$SRC_DIR/main.cpp" \
    "$SRC_DIR/zuma_game.cpp" \
    -o "$BUILD_DIR/zuma_server" \
    -lpthread

if [ $? -eq 0 ]; then
    echo "✅ 编译成功!"
    echo "服务器程序: $BUILD_DIR/zuma_server"
else
    echo "❌ 编译失败!"
    exit 1
fi

echo ""
echo "=== 运行说明 ==="
echo "1. 启动服务器: $BUILD_DIR/zuma_server"
echo "2. 在浏览器中打开: http://localhost:8080"
echo "3. 或使用 run.sh 启动"