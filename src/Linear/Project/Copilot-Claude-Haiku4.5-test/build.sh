#!/bin/bash

# 编译祖玛游戏项目

echo "==================================="
echo "祖玛游戏 - 编译脚本"
echo "==================================="

# 进入项目目录
cd "$(dirname "$0")"

# 创建 build 目录
if [ ! -d "build" ]; then
    mkdir -p build
fi

cd build

# 使用 g++ 编译
echo "正在编译 C++ 源代码..."

g++ -std=c++11 -Wall -Wextra \
    ../src/zuma_game.cpp \
    ../src/main.cpp \
    -o zuma_server

if [ $? -eq 0 ]; then
    echo "✓ 编译成功！"
    echo ""
    echo "可执行文件已生成：build/zuma_server"
    echo ""
    echo "使用方法："
    echo "  cd build"
    echo "  ./zuma_server"
    echo ""
    echo "然后在浏览器中打开: file://$(pwd)/../public/index.html"
else
    echo "✗ 编译失败！"
    exit 1
fi
