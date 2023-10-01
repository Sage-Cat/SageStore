# Add all source files
set(SOURCE_FILES
    main.cpp
)

# Main executable
add_executable(ClientApp ${SOURCE_FILES})

# Link any necessary libraries here
target_link_libraries(ClientApp PRIVATE nlohmann_json::nlohmann_json)
