#!/bin/bash

# 祖玛游戏启动脚本

echo "=========================================="
echo "      祖玛游戏 - Zuma Game"
echo "=========================================="
echo ""

# 检查可执行文件是否存在
if [ ! -f "./zuma_server" ]; then
    echo "正在编译服务器..."
    make clean
    make
    if [ $? -ne 0 ]; then
        echo "编译失败！"
        exit 1
    fi
    echo "编译成功！"
    echo ""
fi

echo "启动服务器..."
echo "请在浏览器中打开: http://localhost:8080"
echo "按 Ctrl+C 停止服务器"
echo ""

./zuma_server
