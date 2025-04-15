# Compiler
CXX = g++
# Compiler flags (Base)
CXXFLAGS = -Wall -std=c++20
# Linker flags
LDFLAGS = -lm

# Source files (Makefile is in the same folder as the sources)
SRCS = main.cpp order_manager.cpp market_snapshot.cpp logger.cpp

# Object files directory
OBJDIR = build

# Create object file names based on source files.
# This converts, for example, "main.cpp" into "build/main.o"
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

# Executable names
EXEC_DBG = phase_3_dbg
EXEC_REL = phase_3_rel
EXEC_PROF = phase_3_prof

# Default target
all: release

# Target for creating the build directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Rule to compile source files into object files.
# The pattern $< stands for the current .cpp file.
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(OPTFLAGS_TARGET) -c $< -o $@

# Debug build (-O0)
debug: OPTFLAGS_TARGET = -O0 -g
debug: $(EXEC_DBG)
	@echo "Running Debug Build ($(EXEC_DBG))..."
	./$(EXEC_DBG) debug 256 5 # Pass build type, N, runs

$(EXEC_DBG): $(OBJS)
	@echo "Linking $(EXEC_DBG)..."
	$(CXX) $(OPTFLAGS_TARGET) $^ -o $@ $(LDFLAGS)

# Profile build (-O2)
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