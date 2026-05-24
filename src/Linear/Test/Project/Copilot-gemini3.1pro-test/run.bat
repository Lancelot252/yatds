@echo off
echo Building Zuma Game Server...
g++ server.cpp -o server.exe -lws2_32
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b %errorlevel%
)
echo Build success. Starting server...
server.exe
