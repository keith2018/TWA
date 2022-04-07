cmake_minimum_required(VERSION 3.3)
project(twa)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3")

FILE(GLOB_RECURSE TWA_SRCS "src/*")
add_executable(twa ${TWA_SRCS})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)