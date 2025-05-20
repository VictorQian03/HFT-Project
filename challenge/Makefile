CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pthread -O3 -g 
BOOST_LIBS = -lboost_system 
LDFLAGS = $(BOOST_LIBS)

all: client server

client: client/client.o
	$(CXX) $(CXXFLAGS) $^ -o client/client $(LDFLAGS)

server: server/server.o
	$(CXX) $(CXXFLAGS) $^ -o server/server $(LDFLAGS)

client/client.o: client/client.cpp client/ConcurrentQueue.h
	$(CXX) $(CXXFLAGS) -c client/client.cpp -o $@

server/server.o: server/main_server.cpp
	$(CXX) $(CXXFLAGS) -c server/main_server.cpp -o $@

clean:
	rm -f client/client server/server client/*.o server/*.o
	rm -f client/client_packet_latency_log.csv client/client_tcp_response_latency_log.csv server/server_events_log.csv