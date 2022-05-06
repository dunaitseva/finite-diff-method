cmake_minimum_required(VERSION 3.22)
project(FiniteDifferenceMethod)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Werror -Wextra -Wall -Wpedantic")

set(PROJECT_DIR project)
set(INCLUDE_DIR ${PROJECT_DIR}/include)
set(SOURCE_DIR ${PROJECT_DIR}/src)

set(PROJECT_BINARY_TARGET computeFDM)
set(FDM_LIB FiniteDifferenceMethodLib)

add_library(
        ${FDM_LIB}
        ${SOURCE_DIR}/Model.cpp
)
target_include_directories(${FDM_LIB} PUBLIC ${INCLUDE_DIR})


add_executable(${PROJECT_BINARY_TARGET} ${PROJECT_DIR}/main.cpp)
target_link_libraries(${PROJECT_BINARY_TARGET} PUBLIC ${FDM_LIB})
target_include_directories(${PROJECT_BINARY_TARGET} PUBLIC ${INCLUDE_DIR})
