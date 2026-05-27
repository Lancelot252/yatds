CXX      ?= clang++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wno-unused-parameter
TARGET   := zuma_server
SRC      := backend/main.cpp
HEADERS  := backend/LinkedList.hpp backend/Queue.hpp backend/Stack.hpp backend/Game.hpp backend/HttpServer.hpp

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) --port 8080 --static ./frontend

clean:
	rm -f $(TARGET)

.PHONY: all run clean
