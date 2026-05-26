# 祖玛游戏 Makefile

CXX = g++
CXXFLAGS = -std=c++11 -O2 -Wall -pthread
TARGET = zuma_server
SRC = server.cpp

# macOS 检测
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    CXX = clang++
endif

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRC) ZumaGame.hpp
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
