CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2

TARGET := build/zuma_server
SOURCES := src/main.cpp src/zuma_game.cpp
OBJECTS := $(SOURCES:src/%.cpp=build/%.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

build/%.o: src/%.cpp src/zuma_game.hpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) 8080

clean:
	rm -rf build
