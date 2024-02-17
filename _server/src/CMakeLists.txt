# Create server executable
add_executable(${SERVER_NAME} ${ALL_SERVER_SOURCES} ${COMMON_SOURCES})

# Include directories
target_include_directories(${SERVER_NAME} PUBLIC ${SERVER_SOURCES_DIR})

# Link libs
target_link_libraries(
    ${SERVER_NAME} 
    PUBLIC
    boost::boost 
    spdlog::spdlog 
    nlohmann_json::nlohmann_json
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
)

# Set the destination directory for the executable
set_target_properties(${SERVER_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SERVER_BUILD_DIR})