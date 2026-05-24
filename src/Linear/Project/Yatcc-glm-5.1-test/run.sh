#!/bin/bash

# 祖玛游戏运行脚本

set -e

PROJECT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
PUBLIC_DIR="$PROJECT_DIR/public"

# 如果没有编译过，先编译
if [ ! -f "$BUILD_DIR/zuma_server" ]; then
    echo "服务器未编译，正在构建..."
    bash "$PROJECT_DIR/build.sh"
fi

PORT=8080

echo "=== 启动祖玛游戏服务器 ==="
echo "端口: $PORT"
echo "静态文件目录: $PUBLIC_DIR"
echo "请在浏览器中打开: http://localhost:$PORT"
echo "按 Ctrl+C 停止服务器"
echo ""

"$BUILD_DIR/zuma_server" --port "$PORT" --public "$PUBLIC_DIR"