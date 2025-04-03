# Compiler
CXX = g++
# Compiler flags
CXXFLAGS = -Wall -std=c++20
# Debug flags
DBGFLAGS = -g
# Optimization flags
OPTFLAGS = -O3
# Profiling flags 
PROFDFLAGS = -pg
# Linker flags
LDFLAGS = -lm
# Source files
SRCS = src/main.cpp src/matrix_ops.cpp src/benchmark.cpp
# Object files directory
OBJDIR = build
# Object files
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
$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(DBGFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS += $(DBGFLAGS)
debug: $(EXEC_DBG)

$(EXEC_DBG): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Release build
release: CXXFLAGS += $(OPTFLAGS)
release: $(EXEC_REL)

$(EXEC_REL): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Profile build (gprof)
profile: CXXFLAGS += $(DBGFLAGS) $(PROFDFLAGS)
profile: LDFLAGS += $(PROFDFLAGS)
profile: $(EXEC_PROF)

$(EXEC_PROF): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Clean target
clean:
	rm -rf $(OBJDIR) $(EXEC_DBG) $(EXEC_REL) $(EXEC_PROF) gmon.out *.txt

.PHONY: all debug release profile clean