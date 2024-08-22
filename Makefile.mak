# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++17

# Directories
INCDIR = include
SRCDIR = src
BUILDDIR = build

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# Object files
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(BUILDDIR)/%.o)

# Target executable
TARGET = SatelliteOrbitSimulation

# Build rules
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Create build directory
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Clean up
clean:
	rm -rf $(BUILDDIR) $(TARGET)

