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

TARGET_REALTIME = real_time_processor
SOURCES_REALTIME = $(SRCDIR)/real_time_processing.cpp \
                   $(SRCDIR)/orderbook.cpp \
                   $(SRCDIR)/optimized_orderbook.cpp
OBJECTS_REALTIME = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/rt_%.o,$(SOURCES_REALTIME))

.PHONY: all clean run_benchmarks run_tests run_realtime

all: $(TARGET_BENCHMARK) $(TARGET_TEST) $(TARGET_REALTIME)

$(TARGET_BENCHMARK): $(OBJECTS_BENCHMARK)
	$(CXX) $(CXXFLAGS_RELEASE) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

$(TARGET_TEST): $(OBJECTS_TEST)
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/test_%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

$(TARGET_REALTIME): $(OBJECTS_REALTIME)
	$(CXX) $(CXXFLAGS_DEBUG) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/rt_%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS_DEBUG) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run_benchmarks: $(TARGET_BENCHMARK)
	./$(TARGET_BENCHMARK)

run_tests: $(TARGET_TEST)
	./$(TARGET_TEST)

run_realtime: $(TARGET_REALTIME)
	./$(TARGET_REALTIME)

clean:
	rm -rf $(OBJDIR) $(TARGET_BENCHMARK) $(TARGET_TEST) $(TARGET_REALTIME)
