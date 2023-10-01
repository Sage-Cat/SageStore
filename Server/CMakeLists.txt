cmake_minimum_required(VERSION 3.10)

project(ServerApp VERSION 1.0)

# Add subdirectory for source code
add_subdirectory(src)

# Add subdirectory for tests
enable_testing()
add_subdirectory(tests)
