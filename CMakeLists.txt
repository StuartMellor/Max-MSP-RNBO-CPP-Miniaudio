cmake_minimum_required(VERSION 3.10)

# Set the C++ standard to at least C++11, which is needed for RNBO
set (CMAKE_CXX_STANDARD 11)

# Set the project name
project(SimpleSine)

# Add the executable
add_executable(SimpleSine main.cpp simplesine.cpp rnbo/RNBO.cpp)

# Include the RNBO headers
target_include_directories(SimpleSine PRIVATE rnbo rnbo/common .)