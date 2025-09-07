cmake_minimum_required(VERSION 3.19.2)
project(Parallel_Mean_Shift_OpenMP)
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(include)

find_package(OpenMP REQUIRED)

add_executable(Parallel_Mean_Shift_OpenMP
        main.cpp
        alias.h
        options.h
        image.cpp
        image.h
        image_arr.cpp
        image_arr.h
        image_aos.cpp
        image_aos.h
        image_soa.cpp
        image_soa.h
        benchmark.cpp)

target_link_libraries(Parallel_Mean_Shift_OpenMP ppm_io OpenMP::OpenMP_CXX)

