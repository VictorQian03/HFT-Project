CXX        := clang++
CXXFLAGS   := -std=c++20 -stdlib=libc++ -O3 -Wall -Wextra -pthread
LDFLAGS    := -stdlib=libc++

TARGET          := hft_sim
SRCS            := src/main.cpp \
                   src/OrderProcessing.cpp \
                   src/Analytics.cpp
OBJS            := $(SRCS:.cpp=.o)

BENCH_TARGET    := hft_benchmark
BENCH_SRCS      := src/BenchmarkMain.cpp \
                   src/OrderProcessing.cpp \
                   src/Analytics.cpp
BENCH_OBJS      := $(BENCH_SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

$(BENCH_TARGET): $(BENCH_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: run
run: $(TARGET)
	./$(TARGET)

.PHONY: bench
bench: $(BENCH_TARGET)
	./$(BENCH_TARGET)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BENCH_OBJS) $(TARGET) $(BENCH_TARGET)