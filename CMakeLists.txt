cmake_minimum_required(VERSION 3.22)
project(FiniteDifferenceMethod)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Werror -Wextra -Wall -Wpedantic")

set(PROJECT_DIR project)
set(INCLUDE_DIR ${PROJECT_DIR}/include)
set(SOURCE_DIR ${PROJECT_DIR}/src)

set(PROJECT_BINARY_TARGET computeFDM)

add_executable(${PROJECT_BINARY_TARGET} ${PROJECT_DIR}/main.cpp)
target_include_directories(${PROJECT_BINARY_TARGET} PUBLIC ${INCLUDE_DIR})
