CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC -I./src $(shell pkg-config --cflags Qt5Widgets)
LIBS = $(shell pkg-config --libs Qt5Widgets) -lsqlite3

# Find all .cpp files under src/
SRC := $(shell find src -name "*.cpp")

# OBJECT files (mirrored folder structure under build/)
OBJ := $(patsubst src/%.cpp, build/%.o, $(SRC))

# Binaries = only top-level .cpp in src/
TOPLEVEL_SRC := $(wildcard src/*.cpp)
BIN := $(patsubst src/%.cpp, %, $(TOPLEVEL_SRC))

# Default target
all: $(BIN)

# Rule to build exe for each toplevel .cpp
%: build/%.o $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ build/$*.o $(filter-out build/$*.o,$(OBJ)) $(LIBS)

# Rule to build all object files
build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build $(BIN)

.PHONY: all clean
