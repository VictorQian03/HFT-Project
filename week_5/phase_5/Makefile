CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -g
LDFLAGS =

TARGET = order_book_benchmark_app

SRCDIR = src
INCDIR = include
OBJDIR = obj

SOURCES = $(SRCDIR)/main.cpp \
          $(SRCDIR)/orderbook.cpp \
          $(SRCDIR)/optimized_orderbook.cpp

OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJDIR) $(TARGET) $(TARGET)_original $(TARGET)_optimized # Clean up potential old targets too