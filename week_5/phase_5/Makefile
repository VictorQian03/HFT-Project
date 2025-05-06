CXX = g++
CXXFLAGS_COMMON = -std=c++17 -Wall -Wextra -I$(INCDIR)
CXXFLAGS_DEBUG = $(CXXFLAGS_COMMON) -g
CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -O3 -DNDEBUG

LDFLAGS =

SRCDIR = src
INCDIR = include
OBJDIR = obj

TARGET_BENCHMARK = order_book_benchmark_app
SOURCES_BENCHMARK = $(SRCDIR)/main.cpp \
                    $(SRCDIR)/orderbook.cpp \
                    $(SRCDIR)/optimized_orderbook.cpp
OBJECTS_BENCHMARK = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES_BENCHMARK))

TARGET_TEST = order_book_tests
SOURCES_TEST = $(SRCDIR)/tests.cpp \
               $(SRCDIR)/orderbook.cpp \
               $(SRCDIR)/optimized_orderbook.cpp 
OBJECTS_TEST = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/test_%.o,$(SOURCES_TEST)) 


.PHONY: all clean run_benchmarks run_tests

all: $(TARGET_BENCHMARK) $(TARGET_TEST)

$(TARGET_BENCHMARK): $(OBJECTS_BENCHMARK)
	$(CXX) $(CXXFLAGS_RELEASE) -o $@ $^ $(LDFLAGS) # Use RELEASE for benchmarks

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

$(TARGET_TEST): $(OBJECTS_TEST)
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^ $(LDFLAGS) # Use DEBUG for tests

$(OBJDIR)/test_%.o: $(SRCDIR)/%.cpp | $(OBJDIR) 
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run_benchmarks: $(TARGET_BENCHMARK)
	./$(TARGET_BENCHMARK)

run_tests: $(TARGET_TEST)
	./$(TARGET_TEST)

clean:
	rm -rf $(OBJDIR) $(TARGET_BENCHMARK) $(TARGET_TEST)