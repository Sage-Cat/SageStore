# Specify resource files
set(RESOURCE_FILES "")

# Create client executable
add_executable(SageStoreClient ${ALL_CLIENT_SOURCES} ${RESOURCE_FILES})

# Include directories
target_include_directories(SageStoreClient PUBLIC ${CLIENT_SOURCES_DIR})

# Link libraries 
target_link_libraries(SageStoreClient Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Network spdlog::spdlog)

# Set the destination directory for the executable
set_target_properties(SageStoreClient PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CLIENT_BUILD_DIR})
