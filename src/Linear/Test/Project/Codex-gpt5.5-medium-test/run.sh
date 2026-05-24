#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"
mkdir -p build
g++ -std=c++17 -Wall -Wextra -O2 src/main.cpp -o build/zuma_server
./build/zuma_server "${1:-8080}"
