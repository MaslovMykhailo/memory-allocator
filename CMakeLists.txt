cmake_minimum_required(VERSION 3.14)
project(MemoryAllocator)

set(CMAKE_CXX_STANDARD 14)

add_executable(MemoryAllocator main.cpp sbrk.cpp sbrk.h memory-block.cpp memory-block.h memory-allocation.cpp memory-allocation.h)