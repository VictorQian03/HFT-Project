cmake_minimum_required(VERSION 3.30)
project(phase3)

set(CMAKE_CXX_STANDARD 20)

add_executable(phase3 main.cpp order_manager.cpp market_snapshot.cpp
        logger.cpp)
