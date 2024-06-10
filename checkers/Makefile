# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17 -I./src/game

# Directories
SRCDIR = src

# Source files
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/src/game.cpp $(SRCDIR)/src/minimax.cpp $(SRCDIR)/src/notation_encoder.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable
EXECUTABLE = a.out

# Default target
all: $(EXECUTABLE)

# Link
$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@

# Compile main.cpp
$(SRCDIR)/main.o: $(SRCDIR)/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile game.cpp
$(GAMEDIR)/game.o: $(GAMEDIR)/game.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

# Format code
format:
	clang-format -i --style llvm $()


# Phony targets
.PHONY: all clean
