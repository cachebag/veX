CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -I/opt/homebrew/opt/sfml/include
LDFLAGS = -L/opt/homebrew/opt/sfml/lib -lsfml-graphics -lsfml-window -lsfml-system

TARGET = game
SOURCES = main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Rule to build the final executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Rule to build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up the generated files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets to prevent conflicts with files of the same name
.PHONY: clean

