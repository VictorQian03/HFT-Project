# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O3 -march=native -Iinclude -pthread

# Directories
SRCDIR := src
TESTDIR := test
OBJDIR := build
TESTOBJDIR := build/test

# Targets
TARGET := hft_app
TEST_TARGET := latency_test

# Sources
SOURCES := \
    $(SRCDIR)/MarketDataFeed.cpp \
    $(SRCDIR)/OrderBook.cpp \
    $(SRCDIR)/MatchingEngine.cpp \
    $(SRCDIR)/OrderManager.cpp \
    $(SRCDIR)/MemoryPool.cpp

# Main application objects
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
MAIN_OBJ := $(OBJDIR)/main.o

# Test objects
TEST_SOURCES := $(TESTDIR)/test_latency.cpp
TEST_OBJECTS := $(patsubst $(TESTDIR)/%.cpp,$(TESTOBJDIR)/%.o,$(TEST_SOURCES))

.PHONY: all clean test

all: $(TARGET) $(TEST_TARGET)

# Link main application
$(TARGET): $(MAIN_OBJ) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Link test application
$(TEST_TARGET): $(TEST_OBJECTS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile main source
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test source
$(TESTOBJDIR)/%.o: $(TESTDIR)/%.cpp | $(TESTOBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build directories exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TESTOBJDIR):
	mkdir -p $(TESTOBJDIR)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET) $(TEST_TARGET)