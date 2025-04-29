# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O3 -march=native -Iinclude -pthread

# Directories
SRCDIR := src
OBJDIR := build

# Target
TARGET := hft_app

# Sources 
SOURCES := \
    $(SRCDIR)/main.cpp \
    $(SRCDIR)/MarketDataFeed.cpp\
     $(SRCDIR)/OrderBook.cpp \
    # $(SRCDIR)/MatchingEngine.cpp \
    # $(SRCDIR)/OrderManager.cpp \
    # $(SRCDIR)/TradeLogger.cpp

# Objects
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: all clean

all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compile
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Ensure build directory exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)