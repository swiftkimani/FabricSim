cmake_minimum_required(VERSION 3.14)
project(FabricSim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure warnings are treated seriously
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# Include directories for headers
include_directories(include)

# ---------------------------------------------------------
# Examples / Sandbox programs (e.g., Module 2 UDP testing)
# ---------------------------------------------------------
add_executable(udp_sender examples/udp_sender.cpp)
add_executable(udp_receiver examples/udp_receiver.cpp)

# (Future modules like the Topology Generator and Simulator core will go here)
