cmake_minimum_required(VERSION 3.23)
project("SageStore")

set(CLIENT_NAME "${PROJECT_NAME}Client")
set(SERVER_NAME "${PROJECT_NAME}Server")

# CMake standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options to build different components
option(BUILD_CLIENT "Build the client application." OFF)
option(BUILD_CLIENT_TESTS "Build the client tests." OFF)
option(BUILD_SERVER "Build the server application." OFF)
option(BUILD_SERVER_TESTS "Build the server tests." OFF)

# Common includes and sources
set(COMMON_DIR "${CMAKE_CURRENT_SOURCE_DIR}/common")
include_directories(${COMMON_DIR})

set(COMMON_SOURCES "${COMMON_DIR}/SpdlogConfig.cpp")

# Conditionally include sub-projects
if(BUILD_CLIENT OR BUILD_CLIENT_TESTS)
    add_subdirectory(_client)
endif()

if(BUILD_SERVER OR BUILD_SERVER_TESTS)
    add_subdirectory(_server)
endif()

# Documentation (if needed)
# find_package(Doxygen)
# if(DOXYGEN_FOUND)
#     add_custom_target(
#         doc ALL
#         COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#         COMMENT "Generating API documentation with Doxygen"
#         VERBATIM
#     )
# endif()

