#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ip/address.hpp> 
#include <boost/asio/ip/address_v4.hpp> 
#include <boost/asio/ip/multicast.hpp>
#include <boost/asio/connect.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <chrono>
#include <iomanip> 

using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using boost::asio::ip::make_address;

const std::string MULTICAST_ADDRESS = "239.255.0.1";
const unsigned short MULTICAST_PORT = 3001;
const unsigned short LISTEN_PORT = 3001; 
const std::string SERVER_ADDRESS = "127.0.0.1"; 
const unsigned short SERVER_PORT = 4000; 

const std::string TRADER_NAME = "Grand Master Oogway";

std::unordered_map<std::string, std::pair<double, double>> current_market_data;
int current_challenge_id = -1;
std::string current_target_ticker;
std::mutex data_mutex; 

boost::asio::io_context io_context;
udp::socket udp_socket(io_context);
boost::asio::streambuf udp_receive_buffer;
udp::endpoint multicast_endpoint;
udp::endpoint sender_endpoint;

void start_udp_receive();
void handle_udp_receive(const boost::system::error_code& error, size_t bytes_transferred);
void process_udp_message(std::istream& is);
void send_tcp_response(int challenge_id, const std::string& ticker, double bid, double ask);

void start_udp_receive() {
    udp_receive_buffer.prepare(1500); 

    udp_socket.async_receive_from(
        udp_receive_buffer.prepare(1500), 
        sender_endpoint, 
        boost::bind(&handle_udp_receive,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void handle_udp_receive(const boost::system::error_code& error, size_t bytes_transferred) {
    if (!error) {
        udp_receive_buffer.commit(bytes_transferred);

        std::istream is(&udp_receive_buffer);
        process_udp_message(is);

        start_udp_receive();

    } else if (error != boost::asio::error::operation_aborted) {
        std::cerr << "[" << TRADER_NAME << "] UDP receive error: " << error.message() << std::endl;
        start_udp_receive();
    }
}

void process_udp_message(std::istream& is) {
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty()) continue; 
        if (line.rfind("SEC|", 0) == 0) {
            std::stringstream ss(line);
            std::string segment;
            std::vector<std::string> segments;

            while(std::getline(ss, segment, '|')) {
                segments.push_back(segment);
            }

            if (segments.size() == 6 && segments[0] == "SEC" && segments[2] == "BID" && segments[4] == "ASK") {
                std::string ticker = segments[1];
                try {
                    double bid = std::stod(segments[3]);
                    double ask = std::stod(segments[5]);

                    std::lock_guard<std::mutex> lock(data_mutex);
                    current_market_data[ticker] = {bid, ask};

                } catch (const std::invalid_argument& e) {
                    std::cerr << "[" << TRADER_NAME << "] Error parsing SEC numeric data: " << line << " - " << e.what() << std::endl;
                } catch (const std::out_of_range& e) {
                    std::cerr << "[" << TRADER_NAME << "] Error parsing SEC numeric data (out of range): " << line << " - " << e.what() << std::endl;
                }
            } else {
                 std::cerr << "[" << TRADER_NAME << "] Warning: Unexpected SEC line format or missing segments: " << line << std::endl;
            }
        }
        else if (line.rfind("CHALLENGE_ID:", 0) == 0) {
            try {
                int id = std::stoi(line.substr(13));
                 {
                    std::lock_guard<std::mutex> lock(data_mutex);
                    if (id != current_challenge_id) {
                        current_challenge_id = id;
                        std::cout << "[" << TRADER_NAME << "] New Challenge ID: " << current_challenge_id << std::endl;
                    }
                 }
            } catch (const std::invalid_argument& e) {
                 std::cerr << "[" << TRADER_NAME << "] Error parsing CHALLENGE_ID: " << line << " - " << e.what() << std::endl;
            } catch (const std::out_of_range& e) {
                 std::cerr << "[" << TRADER_NAME << "] Error parsing CHALLENGE_ID (out of range): " << line << " - " << e.what() << std::endl;
            }
        }
        else if (line.rfind("TARGET:", 0) == 0) {
            std::string target_ticker = line.substr(7);
            {
                std::lock_guard<std::mutex> lock(data_mutex);
                current_target_ticker = target_ticker;
                std::cout << "[" << TRADER_NAME << "] Target Ticker: " << current_target_ticker << " for Challenge ID: " << current_challenge_id << std::endl;

                auto it = current_market_data.find(current_target_ticker);
                if (current_challenge_id != -1 && it != current_market_data.end()) {
                    double bid = it->second.first;
                    double ask = it->second.second;

                    std::cout << "[" << TRADER_NAME << "] Found target data: " << current_target_ticker << " BID=" << bid << " ASK=" << ask << std::endl;
                    send_tcp_response(current_challenge_id, current_target_ticker, bid, ask);

                    current_market_data.clear();
                    current_target_ticker.clear();
                    current_challenge_id = -1; 
                } else {
                    std::cerr << "[" << TRADER_NAME << "] Error: Target ticker " << current_target_ticker
                              << (current_challenge_id == -1 ? " received without a valid Challenge ID." : (" not found in market data for challenge " + std::to_string(current_challenge_id) + "."))
                              << " Accumulated data size: " << current_market_data.size() << std::endl; 
                     current_market_data.clear();
                     current_target_ticker.clear();
                     current_challenge_id = -1;
                }
            }
        }
         else if (line.rfind("WINNER ", 0) == 0 || line.rfind("INCORRECT", 0) == 0) {
            std::cout << "[" << TRADER_NAME << "] Server Response: " << line << std::endl;
         }
        else {
        }
    }
}

void send_tcp_response(int challenge_id, const std::string& ticker, double bid, double ask) {
    auto socket = std::make_shared<tcp::socket>(io_context);
    auto resolver = std::make_shared<tcp::resolver>(io_context);
    resolver->async_resolve(SERVER_ADDRESS, std::to_string(SERVER_PORT),
        [socket, resolver, challenge_id, ticker, bid, ask]( 
            const boost::system::error_code& resolve_error,
            tcp::resolver::results_type results) {

        if (!resolve_error) {
            boost::asio::async_connect(*socket, results,
                [socket, challenge_id, ticker, bid, ask]( 
                    const boost::system::error_code& connect_error,
                    const tcp::resolver::results_type::endpoint_type& endpoint) {

                if (!connect_error) {
                    std::cout << "[" << TRADER_NAME << "] TCP connected to " << endpoint << std::endl;

                    std::ostringstream message_stream;
                    message_stream << "CHALLENGE_RESPONSE "
                                   << challenge_id << " "
                                   << ticker << " "
                                   << std::fixed << std::setprecision(4) << bid << " "
                                   << std::fixed << std::setprecision(4) << ask << " "
                                   << TRADER_NAME << "\n";

                    auto message = std::make_shared<std::string>(message_stream.str()); 
                    boost::asio::async_write(*socket, boost::asio::buffer(*message),
                        [socket, message, challenge_id](
                            const boost::system::error_code& write_error,
                            size_t bytes_transferred) {

                        if (!write_error) {
                            std::cout << "[" << TRADER_NAME << "] TCP sent " << bytes_transferred << " bytes for challenge " << challenge_id << std::endl;
                        } else {
                            std::cerr << "[" << TRADER_NAME << "] TCP write error: " << write_error.message() << std::endl;
                        }
                    });
                } else {
                    std::cerr << "[" << TRADER_NAME << "] TCP connect error: " << connect_error.message() << std::endl;
                }
            });
        } else {
            std::cerr << "[" << TRADER_NAME << "] TCP resolve error: " << resolve_error.message() << std::endl;
        }
    });
}

int main() {
    try {
        udp_socket.open(udp::v4());
        udp_socket.set_option(udp::socket::reuse_address(true));
        udp_socket.bind(udp::endpoint(make_address("0.0.0.0"), LISTEN_PORT ) );
        multicast_endpoint = udp::endpoint(
            make_address(MULTICAST_ADDRESS),
            MULTICAST_PORT
        );
        udp_socket.set_option(
            boost::asio::ip::multicast::join_group(
                make_address(MULTICAST_ADDRESS)
            )
        );
        udp_socket.set_option(boost::asio::ip::multicast::enable_loopback(true));
        std::cout << "[" << TRADER_NAME << "] Client started. Listening for multicast on "
                  << MULTICAST_ADDRESS << ":" << LISTEN_PORT
                  << ", sending TCP to " << SERVER_ADDRESS << ":" << SERVER_PORT << std::endl;
        start_udp_receive();
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "[" << TRADER_NAME << "] Exception: " << e.what() << std::endl;
    }
    return 0;
}