# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
INCLUDES = -Iinclude
TARGET = game

# Sources and objects
SRCDIR = src
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

# Platform detection
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
endif

ifeq ($(UNAME_S),Darwin)
    CXXFLAGS += -I/opt/homebrew/opt/sfml/include
    LDFLAGS = -L/opt/homebrew/opt/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system
endif

# Rule to build the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Rule to build object files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule to clean up the generated files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets to prevent conflicts with files of the same name
.PHONY: clean

