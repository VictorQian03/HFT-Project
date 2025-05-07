CXX = g++
CXXFLAGS_COMMON = -std=c++17 -Wall -Wextra -I$(INCDIR)
CXXFLAGS_DEBUG = $(CXXFLAGS_COMMON) -g
CXXFLAGS_RELEASE = $(CXXFLAGS_COMMON) -O3 -DNDEBUG -Rpass=loop-vectorize
LDFLAGS =

SRCDIR = src
INCDIR = include
OBJDIR = obj

# Hash Table Benchmark
TARGET_HASH_BENCHMARK = robinhood_benchmark_app
SOURCES_HASH_BENCHMARK = $(SRCDIR)/main.cpp \
                        $(SRCDIR)/robinhood_hash_table.cpp
OBJECTS_HASH_BENCHMARK = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES_HASH_BENCHMARK))

# Priority Queue Benchmark
TARGET_PQ_BENCHMARK = priority_queue_benchmark_app
SOURCES_PQ_BENCHMARK = $(SRCDIR)/priority_queue_benchmark.cpp \
                      $(SRCDIR)/order_priority_queue.cpp
OBJECTS_PQ_BENCHMARK = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES_PQ_BENCHMARK))

# Time Series Benchmark
TARGET_TS_BENCHMARK = time_series_benchmark_app
SOURCES_TS_BENCHMARK = $(SRCDIR)/time_series_benchmark.cpp \
                       $(SRCDIR)/time_series_processor.cpp
OBJECTS_TS_BENCHMARK = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES_TS_BENCHMARK))

.PHONY: all clean run_hash_benchmark run_pq_benchmark run_ts_benchmark

all: $(TARGET_HASH_BENCHMARK) $(TARGET_PQ_BENCHMARK) $(TARGET_TS_BENCHMARK)

$(TARGET_HASH_BENCHMARK): $(OBJECTS_HASH_BENCHMARK)
	$(CXX) $(CXXFLAGS_RELEASE) -o $@ $^ $(LDFLAGS)

$(TARGET_PQ_BENCHMARK): $(OBJECTS_PQ_BENCHMARK)
	$(CXX) $(CXXFLAGS_RELEASE) -o $@ $^ $(LDFLAGS)

$(TARGET_TS_BENCHMARK): $(OBJECTS_TS_BENCHMARK)
	$(CXX) $(CXXFLAGS_RELEASE) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS_RELEASE) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run_hash_benchmark: $(TARGET_HASH_BENCHMARK)
	./$(TARGET_HASH_BENCHMARK)

run_pq_benchmark: $(TARGET_PQ_BENCHMARK)
	./$(TARGET_PQ_BENCHMARK)

run_ts_benchmark: $(TARGET_TS_BENCHMARK)
	./$(TARGET_TS_BENCHMARK)

clean:
	rm -rf $(OBJDIR) $(TARGET_HASH_BENCHMARK) $(TARGET_PQ_BENCHMARK) $(TARGET_TS_BENCHMARK)
