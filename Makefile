# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./src

# Libraries
LIBS = -lsqlite3

# Find all .cpp files under src/ recursively
SRC := $(shell find src -name "*.cpp")

# Generate .o names in build folder
OBJ := $(patsubst src/%.cpp, build/%.o, $(SRC))

# Final binary name
TARGET = app

# Default rule
all: $(TARGET)

# Link final output
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) $(LIBS) -o $(TARGET)

# Compile .cpp → .o (mirrors folders!)
build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Remove build products
clean:
	rm -rf build $(TARGET)

.PHONY: all clean
