CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC \
           -I./src $(shell pkg-config --cflags Qt5Widgets)
LIBS = $(shell pkg-config --libs Qt5Widgets) -lsqlite3

# --- Source files ------------------------------------------------------------- 
SRC_CLIENT := $(shell find src/client src/shared -type f -name "*.cpp") \
              src/server/db/queries/user.cpp\
			  src/server/db/model/user.cpp  # TODO: decompose server from client functionality
SRC_SERVER := $(shell find src/server src/shared -type f -name "*.cpp")

# --- Qt Resources -------------------------------------------------------------
RES_QRC = src/shared/resources/resources.qrc
RES_CPP = build/shared/resources/resources_qrc.cpp

# Add resources to client sources
SRC_CLIENT += $(RES_CPP)

# --- Object files -------------------------------------------------------------
# Mirror source folder structure in build/
OBJ_CLIENT := $(patsubst src/%.cpp, build/%.o, $(SRC_CLIENT))
OBJ_CLIENT := $(patsubst build/%.cpp, build/%.o, $(OBJ_CLIENT))
OBJ_SERVER := $(patsubst src/%.cpp, build/%.o, $(SRC_SERVER))

# --- Targets -----------------------------------------------------------------
all: client server

client: $(OBJ_CLIENT)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_CLIENT) $(LIBS)

server: $(OBJ_SERVER)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ_SERVER) -lsqlite3

# --- Build rules -------------------------------------------------------------
# Rule to compile any .cpp in src/ to build/ folder
build/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile Qt resource-generated .cpp
build/%.o: build/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to generate Qt resource .cpp from .qrc
$(RES_CPP): $(RES_QRC)
	mkdir -p $(dir $@)
	rcc -o $@ $<

# Clean build artifacts
clean:
	rm -rf build client server

.PHONY: all clean
