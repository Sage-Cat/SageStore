cmake_minimum_required(VERSION 3.23)
project("SageStore")

set(CLIENT_NAME "${PROJECT_NAME}Client")
set(SERVER_NAME "${PROJECT_NAME}Server")

# CMake standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options to build different components
option(BUILD_CLIENT "Build the client application." OFF)
option(BUILD_SERVER "Build the server application." OFF)
option(BUILD_TESTS "Build tests for client and server." OFF)

# Common includes and sources
set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
include_directories(${COMMON_DIR})

if(BUILD_TESTS)
    enable_testing()
endif()

if(BUILD_CLIENT OR BUILD_TESTS)
    add_subdirectory(_client)
endif()

if(BUILD_SERVER OR BUILD_TESTS)
    add_subdirectory(_server)
endif()