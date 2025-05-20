#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/asio/ip/address.hpp>
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
#include <atomic>
#include <algorithm> 
#include <fstream>  
#include <csignal>   

#include "ConcurrentQueue.h"

using boost::asio::ip::udp;
using boost::asio::ip::tcp;
using boost::asio::ip::make_address;

const std::string MULTICAST_ADDRESS = "239.255.0.1";
const unsigned short MULTICAST_PORT = 3001;
const unsigned short LISTEN_PORT = 3001;
const std::string SERVER_ADDRESS = "127.0.0.1";
const unsigned short SERVER_PORT = 4000;
const std::string TRADER_NAME = "Grand Master Oogway Logfu";

const size_t UDP_PACKET_QUEUE_CAPACITY = 8192;
const unsigned int NUM_WORKER_THREADS = std::max(1u, std::thread::hardware_concurrency() / 2);
const size_t LOG_QUEUE_CAPACITY = 16384;
const size_t LOG_BATCH_SIZE = 1000; 
const std::chrono::milliseconds LOG_FLUSH_INTERVAL(1000);

const std::string PACKET_LATENCY_LOG_FILE = "client_packet_latency_log.csv";
const std::string TCP_RESPONSE_LOG_FILE = "client_tcp_response_latency_log.csv";

boost::asio::io_context io_context_udp;
boost::asio::io_context io_context_tcp;

std::shared_ptr<boost::asio::ip::tcp::socket> persistent_tcp_socket;
udp::socket udp_listen_socket(io_context_udp);
udp::endpoint udp_sender_endpoint;
std::array<char, 65507> udp_raw_receive_buffer;

std::atomic<bool> shutdown_requested{false};

struct QueuedUDPPacket {
    std::string data;
    std::chrono::high_resolution_clock::time_point arrival_time;
    std::chrono::high_resolution_clock::time_point enqueue_time;
};

ConcurrentQueue<QueuedUDPPacket, UDP_PACKET_QUEUE_CAPACITY> udp_packet_queue;
std::vector<std::thread> worker_threads;

std::mutex client_data_mutex;
std::unordered_map<std::string, std::pair<double, double>> current_market_data;
int current_challenge_id = -1;
std::string current_target_ticker;
std::chrono::high_resolution_clock::time_point current_target_parsed_time;

struct ClientPacketLatencyRecord {
    std::chrono::nanoseconds udp_arrival_to_queue_ns;
    std::chrono::nanoseconds queue_duration_ns;
    std::chrono::nanoseconds worker_processing_ns;
};

struct ClientTcpResponseLatencyRecord {
    int challenge_id;
    std::chrono::nanoseconds target_parsed_to_tcp_sent_ns;
};

ConcurrentQueue<ClientPacketLatencyRecord, LOG_QUEUE_CAPACITY> client_packet_latency_log_queue;
ConcurrentQueue<ClientTcpResponseLatencyRecord, LOG_QUEUE_CAPACITY> client_tcp_response_log_queue;

std::ofstream packet_latency_log_stream;
std::ofstream tcp_response_log_stream;

void signal_handler(int signum);
void start_udp_receive();
void handle_udp_receive(const boost::system::error_code& error, size_t bytes_transferred);
void worker_thread_function(int worker_id);
void process_queued_message(int worker_id, const QueuedUDPPacket& packet_info);
void send_tcp_response(int challenge_id, const std::string& ticker, double bid, double ask, std::chrono::high_resolution_clock::time_point target_parsed_time);
void packet_latency_logger_thread_func();
void tcp_response_logger_thread_func();

void signal_handler(int signum) {
    std::cout << "[" << TRADER_NAME << "] Interrupt signal (" << signum << ") received. Shutting down..." << std::endl;
    shutdown_requested = true;
}

void start_udp_receive() {
    if (shutdown_requested) return;
    udp_listen_socket.async_receive_from(
        boost::asio::buffer(udp_raw_receive_buffer),
        udp_sender_endpoint,
        boost::bind(&handle_udp_receive,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
}

void handle_udp_receive(const boost::system::error_code& error, size_t bytes_transferred) {
    auto arrival_time = std::chrono::high_resolution_clock::now();
    if (shutdown_requested) return;

    if (!error && bytes_transferred > 0) {
        QueuedUDPPacket packet;
        packet.data.assign(udp_raw_receive_buffer.data(), bytes_transferred);
        packet.arrival_time = arrival_time;
        packet.enqueue_time = std::chrono::high_resolution_clock::now();
        udp_packet_queue.push(std::move(packet));
        start_udp_receive();
    } else if (error != boost::asio::error::operation_aborted) {
        std::cerr << "[" << TRADER_NAME << "] UDP receive error: " << error.message() << std::endl;
        if (!shutdown_requested) start_udp_receive();
    }
}

void worker_thread_function(int worker_id) {
    std::cout << "[" << TRADER_NAME << "] Worker thread " << worker_id << " started." << std::endl;
    QueuedUDPPacket packet_info;
    ClientPacketLatencyRecord latency_record;

    while (!shutdown_requested || !udp_packet_queue.is_empty()) {
        if (udp_packet_queue.pop(packet_info)) {
            auto dequeue_time = std::chrono::high_resolution_clock::now();
            
            latency_record.udp_arrival_to_queue_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(packet_info.enqueue_time - packet_info.arrival_time);
            latency_record.queue_duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(dequeue_time - packet_info.enqueue_time);
            
            auto processing_start_time = std::chrono::high_resolution_clock::now();
            process_queued_message(worker_id, packet_info);
            auto processing_end_time = std::chrono::high_resolution_clock::now();
            
            latency_record.worker_processing_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(processing_end_time - processing_start_time);
            client_packet_latency_log_queue.push(latency_record);
        } else if (!shutdown_requested) {
            std::this_thread::yield(); 
        }
    }
    std::cout << "[" << TRADER_NAME << "] Worker thread " << worker_id << " stopping." << std::endl;
}

void process_queued_message(int worker_id, const QueuedUDPPacket& packet_info) {
    std::istringstream is(packet_info.data);
    std::string line;

    // 1) Local buffer for this fragment’s SEC updates
    std::vector<std::pair<std::string,std::pair<double,double>>> sec_updates;

    // 2) Flags + parsed values
    bool challenge_id_found = false;
    bool target_found       = false;
    int  parsed_challenge_id;
    std::string parsed_target_ticker;
    auto  parsed_time = std::chrono::high_resolution_clock::now();

    // 3) First pass: just read & stash
    while (std::getline(is, line)) {
        if (line.rfind("SEC|", 0) == 0) {
            std::stringstream ss(line);
            std::string seg;
            std::vector<std::string> f;
            while (std::getline(ss, seg, '|')) f.push_back(seg);
            if (f.size() == 6 && f[2]=="BID" && f[4]=="ASK") {
                try {
                    double bid = std::stod(f[3]);
                    double ask = std::stod(f[5]);
                    sec_updates.emplace_back(f[1], std::make_pair(bid,ask));
                } catch(...) {  }
            }
        }
        else if (line.rfind("CHALLENGE_ID:",0)==0) {
            try {
                parsed_challenge_id = std::stoi(line.substr(13));
                challenge_id_found = true;
            } catch(...) { }
        }
        else if (line.rfind("TARGET:",0)==0) {
            parsed_target_ticker = line.substr(7);
            target_found = true;
        }
    }

    // 4) Now apply under lock
    {
        std::lock_guard<std::mutex> lock(client_data_mutex);

        // a) new challenge -> clear
        if (challenge_id_found &&
            parsed_challenge_id != current_challenge_id)
        {
            current_market_data.clear();
            current_target_ticker.clear();
            current_challenge_id = parsed_challenge_id;
        }

        // b) apply all SEC updates from this fragment
        for (auto &u : sec_updates) {
            current_market_data[u.first] = u.second;
        }

        // c) record TARGET if seen
        if (target_found) {
            current_target_ticker   = parsed_target_ticker;
            current_target_parsed_time = parsed_time;
        }
    }

    // 5) If we now have both a challenge and the target’s quote, respond:
    if (current_challenge_id != -1 && !current_target_ticker.empty()) {
        auto it = current_market_data.find(current_target_ticker);
        if (it != current_market_data.end()) {
            send_tcp_response(
                current_challenge_id,
                current_target_ticker,
                it->second.first,
                it->second.second,
                current_target_parsed_time
            );
            // reset for next challenge
            std::lock_guard<std::mutex> lock(client_data_mutex);
            current_market_data.clear();
            current_target_ticker.clear();
        }
    }
}

void send_tcp_response(int challenge_id, const std::string& ticker, double bid, double ask, std::chrono::high_resolution_clock::time_point target_parsed_time) {
    if (!persistent_tcp_socket || !persistent_tcp_socket->is_open() || shutdown_requested) {
        return;
    }

    std::ostringstream message_stream;
    message_stream << "CHALLENGE_RESPONSE " << challenge_id << " " << ticker << " "
                   << std::fixed << std::setprecision(4) << bid << " "
                   << std::fixed << std::setprecision(4) << ask << " "
                   << TRADER_NAME << "\n";
    auto msg_payload = std::make_shared<std::string>(message_stream.str());

    boost::asio::async_write(
        *persistent_tcp_socket,
        boost::asio::buffer(*msg_payload),
        [msg_payload, challenge_id, target_parsed_time](boost::system::error_code ec, std::size_t /*bytes_transferred*/) {
            if (!ec) {
                auto tcp_sent_time = std::chrono::high_resolution_clock::now();
                ClientTcpResponseLatencyRecord latency_rec;
                latency_rec.challenge_id = challenge_id;
                latency_rec.target_parsed_to_tcp_sent_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(tcp_sent_time - target_parsed_time);
                client_tcp_response_log_queue.push(latency_rec);
            } else if (ec != boost::asio::error::operation_aborted) {
                std::cerr << "[" << TRADER_NAME << "] TCP write error for challenge " << challenge_id << ": " << ec.message() << std::endl;
            }
        }
    );
}

void packet_latency_logger_thread_func() {
    std::cout << "[" << TRADER_NAME << "] Packet Latency Logger thread started." << std::endl;
    std::vector<ClientPacketLatencyRecord> batch;
    batch.reserve(LOG_BATCH_SIZE);
    auto last_flush_time = std::chrono::steady_clock::now();

    while (!shutdown_requested || !client_packet_latency_log_queue.is_empty()) {
        ClientPacketLatencyRecord record;
        if (client_packet_latency_log_queue.pop(record)) {
            batch.push_back(record);
        }

        bool batch_full = batch.size() >= LOG_BATCH_SIZE;
        bool flush_interval_elapsed = std::chrono::steady_clock::now() - last_flush_time > LOG_FLUSH_INTERVAL;
        bool shutting_down_and_has_data = shutdown_requested && !batch.empty() && client_packet_latency_log_queue.is_empty();


        if ((batch_full || (flush_interval_elapsed && !batch.empty()) || shutting_down_and_has_data) && packet_latency_log_stream.is_open()) {
            for (const auto& rec : batch) {
                packet_latency_log_stream << rec.udp_arrival_to_queue_ns.count() << ","
                                          << rec.queue_duration_ns.count() << ","
                                          << rec.worker_processing_ns.count() << "\n";
            }
            packet_latency_log_stream.flush(); 
            batch.clear();
            last_flush_time = std::chrono::steady_clock::now();
        } else if (!batch.empty() && !client_packet_latency_log_queue.pop(record) && !shutdown_requested) { 
             std::this_thread::sleep_for(std::chrono::milliseconds(10)); 
        } else if (batch.empty() && !shutdown_requested) {
             std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
         if (shutdown_requested && client_packet_latency_log_queue.is_empty() && batch.empty()) break; 
    }
    if(packet_latency_log_stream.is_open()) packet_latency_log_stream.flush();
    std::cout << "[" << TRADER_NAME << "] Packet Latency Logger thread stopping." << std::endl;
}

void tcp_response_logger_thread_func() {
    std::cout << "[" << TRADER_NAME << "] TCP Response Logger thread started." << std::endl;
    std::vector<ClientTcpResponseLatencyRecord> batch;
    batch.reserve(LOG_BATCH_SIZE);
    auto last_flush_time = std::chrono::steady_clock::now();

    while (!shutdown_requested || !client_tcp_response_log_queue.is_empty()) {
        ClientTcpResponseLatencyRecord record;
        if (client_tcp_response_log_queue.pop(record)) {
            batch.push_back(record);
        }
        
        bool batch_full = batch.size() >= LOG_BATCH_SIZE;
        bool flush_interval_elapsed = std::chrono::steady_clock::now() - last_flush_time > LOG_FLUSH_INTERVAL;
        bool shutting_down_and_has_data = shutdown_requested && !batch.empty() && client_tcp_response_log_queue.is_empty();

        if ((batch_full || (flush_interval_elapsed && !batch.empty()) || shutting_down_and_has_data) && tcp_response_log_stream.is_open()) {
            for (const auto& rec : batch) {
                tcp_response_log_stream << rec.challenge_id << ","
                                        << rec.target_parsed_to_tcp_sent_ns.count() << "\n";
            }
            tcp_response_log_stream.flush();
            batch.clear();
            last_flush_time = std::chrono::steady_clock::now();
        } else if (!batch.empty() && !client_tcp_response_log_queue.pop(record) && !shutdown_requested) {
             std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } else if (batch.empty() && !shutdown_requested) {
             std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
        if (shutdown_requested && client_tcp_response_log_queue.is_empty() && batch.empty()) break;
    }
    if(tcp_response_log_stream.is_open()) tcp_response_log_stream.flush();
    std::cout << "[" << TRADER_NAME << "] TCP Response Logger thread stopping." << std::endl;
}

int main() {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    std::cout << "[" << TRADER_NAME << "] Client starting with " << NUM_WORKER_THREADS << " worker thread(s)." << std::endl;

    packet_latency_log_stream.open(PACKET_LATENCY_LOG_FILE, std::ios::out | std::ios::trunc);
    if (packet_latency_log_stream.is_open()) {
        packet_latency_log_stream << "udp_arrival_to_queue_ns,queue_duration_ns,worker_processing_ns\n";
    } else {
        std::cerr << "[" << TRADER_NAME << "] ERROR: Cannot open " << PACKET_LATENCY_LOG_FILE << std::endl;
        return 1;
    }

    tcp_response_log_stream.open(TCP_RESPONSE_LOG_FILE, std::ios::out | std::ios::trunc);
    if (tcp_response_log_stream.is_open()) {
        tcp_response_log_stream << "challenge_id,target_parsed_to_tcp_sent_ns\n";
    } else {
        std::cerr << "[" << TRADER_NAME << "] ERROR: Cannot open " << TCP_RESPONSE_LOG_FILE << std::endl;
        return 1;
    }

    std::thread packet_logger_th;
    std::thread tcp_logger_th;
    std::thread tcp_io_thread_obj; 
    std::thread udp_io_thread_obj; 

    try {
        udp_listen_socket.open(udp::v4());
        udp_listen_socket.set_option(udp::socket::reuse_address(true));
        udp_listen_socket.bind(udp::endpoint(make_address("0.0.0.0"), LISTEN_PORT));
        udp_listen_socket.set_option(boost::asio::ip::multicast::join_group(make_address(MULTICAST_ADDRESS)));

    persistent_tcp_socket = std::make_shared<tcp::socket>(io_context_tcp);
    tcp::resolver resolver(io_context_tcp);
    auto endpoints = resolver.resolve(SERVER_ADDRESS, std::to_string(SERVER_PORT));
    boost::system::error_code ec;

    // Retry loop
    while (!shutdown_requested) {
        for (auto& ep : endpoints) {
            persistent_tcp_socket->close();
            persistent_tcp_socket->open(tcp::v4());
            persistent_tcp_socket->bind(tcp::endpoint(tcp::v4(), 0)); 
            persistent_tcp_socket->connect(ep, ec);
            if (!ec) break;
        }
        if (!ec) break;

        std::cerr << "[" << TRADER_NAME << "] Failed to connect: "
                  << ec.message()
                  << ". Retrying in 1s...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (ec) {
        std::cerr << "[" << TRADER_NAME << "] Could not connect—exiting.\n";
        return 1;
    }

    persistent_tcp_socket->set_option(tcp::no_delay(true));
    persistent_tcp_socket->set_option(boost::asio::socket_base::keep_alive(true));
    std::cout << "[" << TRADER_NAME << "] Connected to TCP server.\n";

        start_udp_receive();

        for (unsigned int i = 0; i < NUM_WORKER_THREADS; ++i) {
            worker_threads.emplace_back(worker_thread_function, i);
        }

        packet_logger_th = std::thread(packet_latency_logger_thread_func);
        tcp_logger_th = std::thread(tcp_response_logger_thread_func);
        
        tcp_io_thread_obj = std::thread([&]() {
            boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_tcp(io_context_tcp.get_executor());
            io_context_tcp.run();
            std::cout << "[" << TRADER_NAME << "] TCP I/O thread finished." << std::endl;
        });
        
        udp_io_thread_obj = std::thread([&]() {
            boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work_guard_udp(io_context_udp.get_executor());
            io_context_udp.run();
            std::cout << "[" << TRADER_NAME << "] UDP I/O thread finished." << std::endl;
        });

        while (!shutdown_requested) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

    } catch (std::exception& e) {
        std::cerr << "[" << TRADER_NAME << "] Exception in main: " << e.what() << std::endl;
        shutdown_requested = true;
    }

    std::cout << "[" << TRADER_NAME << "] Shutdown sequence initiated." << std::endl;

    io_context_udp.stop();
    io_context_tcp.stop();

    if (udp_io_thread_obj.joinable()) udp_io_thread_obj.join();
    if (tcp_io_thread_obj.joinable()) tcp_io_thread_obj.join();
    std::cout << "[" << TRADER_NAME << "] I/O threads joined." << std::endl;
    
    for (auto& wt : worker_threads) {
        if (wt.joinable()) wt.join();
    }
    std::cout << "[" << TRADER_NAME << "] Worker threads joined." << std::endl;

    if (packet_logger_th.joinable()) packet_logger_th.join();
    if (tcp_logger_th.joinable()) tcp_logger_th.join();
    std::cout << "[" << TRADER_NAME << "] Logger threads joined." << std::endl;

    if (udp_listen_socket.is_open()) {
        boost::system::error_code ec;
        udp_listen_socket.cancel(ec); 
        udp_listen_socket.close(ec);
    }
    if (persistent_tcp_socket && persistent_tcp_socket->is_open()) {
        boost::system::error_code ec;
        persistent_tcp_socket->shutdown(tcp::socket::shutdown_both, ec);
        persistent_tcp_socket->close(ec);
    }
    
    if (packet_latency_log_stream.is_open()) packet_latency_log_stream.close();
    if (tcp_response_log_stream.is_open()) tcp_response_log_stream.close();
    std::cout << "[" << TRADER_NAME << "] Log files closed." << std::endl;

    std::cout << "[" << TRADER_NAME << "] Client shut down gracefully." << std::endl;
    return 0;
}