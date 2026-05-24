CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET = zuma_server
SRCDIR = src
BUILDDIR = build

SRCS = $(SRCDIR)/main.cpp $(SRCDIR)/zuma_game.cpp
OBJS = $(BUILDDIR)/main.o $(BUILDDIR)/zuma_game.o

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BUILDDIR)/$(TARGET) $(OBJS)
	@echo "✅ 编译完成: $(BUILDDIR)/$(TARGET)"

$(BUILDDIR)/main.o: $(SRCDIR)/main.cpp $(SRCDIR)/zuma_game.hpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/main.cpp -o $@

$(BUILDDIR)/zuma_game.o: $(SRCDIR)/zuma_game.cpp $(SRCDIR)/zuma_game.hpp
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/zuma_game.cpp -o $@

run: all
	$(BUILDDIR)/$(TARGET)

clean:
	rm -rf $(BUILDDIR)/*.o $(BUILDDIR)/$(TARGET)
	@echo "🧹 清理完成"