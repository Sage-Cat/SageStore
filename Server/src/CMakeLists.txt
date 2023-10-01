# Add all source files
set(SOURCE_FILES
    main.cpp
)

# Main executable
add_executable(ServerApp ${SOURCE_FILES})

# Link any necessary libraries here
target_link_libraries(ServerApp PRIVATE sqlite3)
