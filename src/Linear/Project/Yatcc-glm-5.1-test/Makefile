CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
LDFLAGS = -lpthread

SRC_DIR = src
BUILD_DIR = build
PUBLIC_DIR = public

SRCS = $(SRC_DIR)/main.cpp $(SRC_DIR)/zuma_game.cpp
TARGET = $(BUILD_DIR)/zuma_server

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS) $(SRC_DIR)/zuma_game.hpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	./$(TARGET) --port 8080 --public $(PUBLIC_DIR)