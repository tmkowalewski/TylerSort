# Makefile for TylerSort project

# Directories
SRC_DIR  := src
OBJ_DIR  := obj
BIN_DIR  := bin

# Compiler and flags
CXX       := g++
CXXFLAGS  := `root-config --cflags` -I~/.local/include -fPIC
LDFLAGS   := `root-config --libs` -L~/.local/lib -lCASort -Wl,-rpath,~/.local/lib
DEBUGFLAGS := -g -O0 -fsanitize=thread
DEBUGLDFLAGS := -fsanitize=thread

# Target executable name
TARGET := $(BIN_DIR)/TylerSort

# Source and object files
SOURCES  := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS  := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Debug target
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: LDFLAGS += $(DEBUGLDFLAGS)
debug: clean all

# Link object files into the executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# A quick test target
test: $(TARGET)
	$(TARGET) ~/TUNL/Data/NRF/70Ge/energy_calibration/calibrations ../../example_data ../../example_data 196 out.196.root

.PHONY: all clean debug