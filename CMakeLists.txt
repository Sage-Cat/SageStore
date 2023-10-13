cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SageStore)

# Recursively include sub-projects
add_subdirectory(_client)
add_subdirectory(_server)
