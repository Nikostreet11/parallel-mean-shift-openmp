cmake_minimum_required(VERSION 3.19.2)
project(Parallel_Mean_Shift-OpenMP)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(include)

find_package(OpenMP REQUIRED)

add_executable(Parallel_Mean_Shift-OpenMP main.cpp)

target_link_libraries(Parallel_Mean_Shift-OpenMP ppm_io OpenMP::OpenMP_CXX)

