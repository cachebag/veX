# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
INCLUDES = -Iinclude

# Platform detection
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)
    LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
    CXXFLAGS += -I/usr/include
    LIBPATHS = -L/usr/lib/x86_64-linux-gnu
endif

ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lsfml-network
    CXXFLAGS += -I/opt/homebrew/opt/sfml/include
    LIBPATHS = -L/opt/homebrew/opt/sfml/lib
endif

# Sources and objects
SRCDIR = src
OBJDIR = obj
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

TARGET = game

# Rule to build the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJECTS) -o $(TARGET) $(LIBPATHS) $(LDFLAGS)

# Rule to build object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Create obj directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Rule to clean up the generated files
clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf $(OBJDIR)

# Phony targets to prevent conflicts with files of the same name
.PHONY: clean

