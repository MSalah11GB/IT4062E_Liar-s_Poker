CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC -I./src $(shell pkg-config --cflags Qt5Widgets)
LIBS = $(shell pkg-config --libs Qt5Widgets) -lsqlite3

# All cpp files in src/
SRC := $(wildcard src/*.cpp)

# Each .cpp becomes a binary with the same name (without .cpp)
BIN := $(patsubst src/%.cpp, %, $(SRC))

all: $(BIN)

# Compile each cpp into its own executable
%: src/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)

clean:
	rm -f $(BIN)

.PHONY: all clean
