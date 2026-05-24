#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$ROOT_DIR/build"

mkdir -p "$BUILD_DIR"

g++ -std=c++17 -O2 -Wall -Wextra -pedantic \
  "$ROOT_DIR/src/main.cpp" \
  -o "$BUILD_DIR/zuma_server"

"$BUILD_DIR/zuma_server"
