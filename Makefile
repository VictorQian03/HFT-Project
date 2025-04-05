# Compiler
CXX = g++
# Compiler flags (Base)
CXXFLAGS = -Wall -std=c++20
# Linker flags
LDFLAGS = -lm
# Source files directory structure assumed 
SRCS = src/main.cpp src/matrix_ops.cpp src/benchmark.cpp
# Object files directory
OBJDIR = build
# Create object file names based on source files
OBJS = $(patsubst src/%.cpp, $(OBJDIR)/%.o, $(SRCS))

# Executable names
EXEC_DBG = matrix_app_dbg
EXEC_REL = matrix_app_rel
EXEC_PROF = matrix_app_prof 

# Default target
all: release

# Target for creating the build directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Rule to compile source files into object files
# Uses target-specific variable $(OPTFLAGS_TARGET) for optimization level
$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS_TARGET) -c $< -o $@

# Debug build (-O0)
debug: OPTFLAGS_TARGET = -O0 -g
debug: $(EXEC_DBG)
	@echo "Running Debug Build ($(EXEC_DBG))..."
	./$(EXEC_DBG) debug 256 5 # Pass build type, N, runs

$(EXEC_DBG): $(OBJS)
	@echo "Linking $(EXEC_DBG)..."
	$(CXX) $(OPTFLAGS_TARGET) $^ -o $@ $(LDFLAGS)

# Profile build (-02) 
profile: OPTFLAGS_TARGET = -O2 -g -pg 
profile: LDFLAGS += -pg
profile: $(EXEC_PROF)
	@echo "Running Profile Build ($(EXEC_PROF))..."
	./$(EXEC_PROF) profile 256 5 # Pass build type, N, runs

$(EXEC_PROF): $(OBJS)
	@echo "Linking $(EXEC_PROF)..."
	$(CXX) $(OPTFLAGS_TARGET) $^ -o $@ $(LDFLAGS)

# Release build (-O3)
release: OPTFLAGS_TARGET = -O3 
release: $(EXEC_REL)
	@echo "Running Release Build ($(EXEC_REL))..."
	./$(EXEC_REL) release 256 5 # Pass build type, N, runs

$(EXEC_REL): $(OBJS)
	@echo "Linking $(EXEC_REL)..."
	$(CXX) $(OPTFLAGS_TARGET) $^ -o $@ $(LDFLAGS)

# Clean target
clean:
	@echo "Cleaning build files..."
	rm -rf $(OBJDIR) $(EXEC_DBG) $(EXEC_REL) $(EXEC_PROF) gmon.out *.txt *.o

# Phony targets
.PHONY: all debug release clean