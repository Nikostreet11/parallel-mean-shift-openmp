cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 11)

project(Parallel_Mean_Shift)

add_subdirectory(libs)

add_executable(Parallel_Mean_Shift main.cpp)

target_link_libraries(Parallel_Mean_Shift PRIVATE ppm_io)

