cmake_minimum_required(VERSION 3.15)
project(first_fit_allocation)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(first_fit_allocation
        main.cpp
        memory-allocation.cpp
        memory-allocation.h
        memory-block.cpp
        memory-block.h
        sbrk.cpp
        sbrk.h)
