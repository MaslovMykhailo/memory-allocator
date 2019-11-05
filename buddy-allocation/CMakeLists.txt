cmake_minimum_required(VERSION 3.15)
project(buddy_allocation)

set(CMAKE_CXX_STANDARD 14)

add_executable(buddy_allocation main.cpp BlocksList.cpp BlocksList.h MemoryAllocator.cpp MemoryAllocator.h)