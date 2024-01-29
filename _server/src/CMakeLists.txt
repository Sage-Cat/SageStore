# Create server executable
add_executable(SageStoreServer ${ALL_SERVER_SOURCES} ${COMMON_SOURCES})

# Include directories
target_include_directories(SageStoreServer PUBLIC ${SERVER_SOURCES_DIR})

# link libs
target_link_libraries(SageStoreServer boost::boost spdlog::spdlog)