cmake_minimum_required(VERSION 3.10)
project(Socketv2SecureTestServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Werror -Wextra)
include_directories(${CMAKE_SOURCE_DIR}/../../../include)
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/socket-v2-secure-test-server.cpp")

if( NOT SRC_FILES )
    message(FATAL_ERROR "Source files couldn't find")
endif()

add_executable(socket-v2-secure-test-server ${SRC_FILES})

if(WIN32)
    target_link_libraries(socket-v2-secure-test-server PRIVATE ws2_32)
elseif(UNIX)
    find_package(Threads REQUIRED)
    target_link_libraries(socket-v2-secure-test-server PRIVATE pthread)
endif()

if(WIN32)
    message(STATUS "Building on Windows...")
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building on Linux/Bsd...")
endif()