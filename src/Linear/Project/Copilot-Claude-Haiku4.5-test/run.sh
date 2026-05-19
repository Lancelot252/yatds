#!/bin/bash

# 运行祖玛游戏

echo "==================================="
echo "祖玛游戏 - 运行脚本"
echo "==================================="

cd "$(dirname "$0")"

# 检查是否已编译
if [ ! -f "build/zuma_server" ]; then
    echo "未找到可执行文件，正在编译..."
    bash build.sh
    if [ $? -ne 0 ]; then
        exit 1
    fi
fi

echo ""
echo "启动祖玛游戏服务器..."
echo ""

# 运行服务器
cd build
./zuma_server

echo ""
echo "服务器已启动！"
