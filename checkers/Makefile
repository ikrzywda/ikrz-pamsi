# Define the compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Define the directories
SRCDIR := src
BUILDDIR := build
BINDIR := src/bin

# Define the target binary and its source files
TARGET := $(BUILDDIR)/bin/checkers_client
SRCS := $(wildcard $(SRCDIR)/client/*.cpp) $(wildcard $(SRCDIR)/checkers/*.cpp) $(BINDIR)/checkers_client.cpp
HEADERS := $(wildcard $(SRCDIR)/client/*.h) $(wildcard $(SRCDIR)/checkers/*.h) $(wildcard $(SRCDIR)/include/*.h)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Default target
all: $(TARGET)

# Build the client binary
$(TARGET): $(OBJS) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^
	cp $@ ./   # Copy the executable to the project's root directory


# Compile source files to object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Create the build directory if it doesn't exist
$(BUILDDIR):
	mkdir -p $(BUILDDIR) 
	mkdir -p $(BUILDDIR)/client 
	mkdir -p $(BUILDDIR)/checkers
	mkdir -p $(BUILDDIR)/bin

# Format the source and header files with clang-format using LLVM style
format:
	clang-format -i --style=llvm $(SRCS) $(HEADERS)

# Phony targets
.PHONY: all clean format

# Clean up the build artifacts
clean:
	rm -rf $(BUILDDIR)

# Ensure that .cpp and .h files are not interpreted as actual targets
.PHONY: $(SRCS) $(HEADERS)
