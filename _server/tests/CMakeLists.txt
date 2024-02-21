set(SERVER_UNIT_TESTS_DIR "${SERVER_TESTS_DIR}/unit")

# Create a static library from the Client source files
add_library(ServerTestLib STATIC ${ALL_SERVER_SOURCES})
target_include_directories(ServerTestLib PUBLIC ${SERVER_SOURCES_DIR})
target_link_libraries(ServerTestLib spdlog::spdlog)

add_subdirectory(unit)