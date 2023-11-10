cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(SageStore)

enable_testing()
option(BUILD_TESTING "Build the testing tree." ON)

# Recursively include sub-projects
add_subdirectory(_client)
add_subdirectory(_server)

find_package(Doxygen)

if(DOXYGEN_FOUND)
    add_custom_target(
        doc ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )

endif()