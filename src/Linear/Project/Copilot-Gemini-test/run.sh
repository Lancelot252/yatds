#!/bin/bash
cd "$(dirname "$0")"
# 编译
clang++ -std=c++11 main.cpp -o zuma_server -pthread
# 运行
./zuma_server
