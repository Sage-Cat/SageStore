# Create server executable
add_executable(${SERVER_NAME} ${ALL_SERVER_SOURCES})

# Include directories
target_include_directories(${SERVER_NAME} PUBLIC ${SERVER_SOURCES_DIR})

# Link libs
target_link_libraries(
    ${SERVER_NAME} 
    PUBLIC
    common
    boost_common
    spdlog::spdlog 
    SQLite::SQLite3
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
)

# Set the destination directory for the executable
set_target_properties(${SERVER_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SERVER_BUILD_DIR})

# Database setup
target_compile_definitions(${SERVER_NAME} PRIVATE
    DB_PATH="${SERVER_BUILD_DIR}/db.sl3"
    CREATE_DB_SQL_FILE_PATH="${SERVER_BUILD_DIR}/scripts/create_db.sql"
)
add_custom_command(
    TARGET ${SERVER_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${SERVER_BUILD_DIR}/scripts"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/scripts/create_db.sql" "${SERVER_BUILD_DIR}/scripts/create_db.sql"
    COMMENT "Copying db_create.sql to build directory"
)

# Compiler flags for GCC/MinGW
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${SERVER_NAME} PRIVATE -Wall -Wextra -Werror)
endif()