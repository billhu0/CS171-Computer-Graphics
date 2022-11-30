cmake_minimum_required(VERSION 3.16)
project(CS171-Hw3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(OpenMP)

option(TESTBYGITHUB "test by GitHub" OFF)
if(TESTBYGITHUB)
    add_definitions(-DTESTBYGITHUB)
endif()

add_subdirectory(libs)
add_subdirectory(src)

add_executable(${PROJECT_NAME}-main src/main.cpp)

target_link_libraries(${PROJECT_NAME}-main PRIVATE renderer)