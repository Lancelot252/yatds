#!/bin/bash
# 祖玛游戏 - 快速启动脚本
set -e

echo "🔨 编译祖玛游戏服务器..."
make clean && make all

echo ""
echo "🎮 启动服务器..."
echo "请在浏览器打开: http://localhost:8080"
echo "按 Ctrl+C 停止服务器"
echo ""

./build/zuma_server