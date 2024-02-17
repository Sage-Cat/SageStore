# Specify resource files
set(RESOURCE_FILES "")

# Create client executable
add_executable(${CLIENT_NAME} ${ALL_CLIENT_SOURCES} ${RESOURCE_FILES} ${COMMON_SOURCES})

# Include directories
target_include_directories(${CLIENT_NAME} PUBLIC ${CLIENT_SOURCES_DIR})

# Link libraries 
target_link_libraries(${CLIENT_NAME} Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network spdlog::spdlog)

# Set the destination directory for the executable
set_target_properties(${CLIENT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CLIENT_BUILD_DIR})
