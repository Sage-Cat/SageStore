# Create server executable
add_executable(SageStoreServer ${ALL_SERVER_SOURCES} ${COMMON_SOURCES})

# Include directories
target_include_directories(SageStoreServer PUBLIC ${SERVER_SOURCES_DIR})

# Link libs
target_link_libraries(
    SageStoreServer 
    PUBLIC
    boost::boost 
    spdlog::spdlog 
    nlohmann_json::nlohmann_json
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
)

# Set the destination directory for the executable
set_target_properties(SageStoreServer PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SERVER_BUILD_DIR})