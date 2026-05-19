@echo off
setlocal
pushd %~dp0

echo Building Zuma game backend...
g++ server.cpp -std=c++17 -O2 -lws2_32 -o zuma_server.exe
if errorlevel 1 (
    echo Build failed.
    popd
    exit /b 1
)

echo Starting server on http://localhost:8080
zuma_server.exe

popd