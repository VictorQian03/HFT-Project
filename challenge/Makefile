CXX = g++

CXXSTD = -std=c++20

BOOST_INCLUDE = /opt/homebrew/include
BOOST_LIB = /opt/homebrew/lib

CXXFLAGS = $(CXXSTD) -I$(BOOST_INCLUDE) -O3 -Wall -Wextra 

LDFLAGS = -L$(BOOST_LIB)

LDLIBS = -lboost_system -lpthread

SERVER_EXEC = server/hft_server
CLIENT_EXEC = client/hft_client

SERVER_SRC = server/main_server.cpp
CLIENT_SRC = client/client.cpp

.DEFAULT_GOAL := all

.PHONY: all clean server client

all: $(SERVER_EXEC) $(CLIENT_EXEC)

$(SERVER_EXEC): $(SERVER_SRC)
	@echo "Compiling server: $< -> $@"
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

$(CLIENT_EXEC): $(CLIENT_SRC)
	@echo "Compiling client: $< -> $@"
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS) $(LDLIBS)

server: $(SERVER_EXEC)
client: $(CLIENT_EXEC)

clean:
	@echo "Cleaning..."
	rm -f $(SERVER_EXEC) $(CLIENT_EXEC)
	@echo "Clean complete."