# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

# SFML Libraries
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# NFD Library
NFD_LIB = lib/libnfd.a

# GTK Libraries
GTK_LIBS = `pkg-config --cflags --libs gtk+-3.0`

# Directories
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
LEVELS_DIR = levels
ASSETS_DIR = assets

# Output executable
TARGET = game

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(SFML_LIBS) $(NFD_LIB) $(GTK_LIBS)

# Compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create object directory if not exists
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean up
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets
.PHONY: all clean

