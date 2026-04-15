set(DEBUG_CLI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DebugCliRunner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ServerController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/UiAutomation.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)

add_executable(SageStoreDebugCli ${DEBUG_CLI_SOURCES})

target_include_directories(SageStoreDebugCli
    PUBLIC
        ${CLIENT_SOURCES_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(SageStoreDebugCli
    common
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Network
    Qt6::Test
    spdlog::spdlog
)

set_target_properties(SageStoreDebugCli PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CLIENT_BUILD_DIR}
)

target_compile_definitions(SageStoreDebugCli PRIVATE
    SAGESTORE_DEBUGCLI_DEFAULT_SERVER_BIN="${CMAKE_BINARY_DIR}/_server/${SERVER_NAME}${CMAKE_EXECUTABLE_SUFFIX}"
    SAGESTORE_DEBUGCLI_DEFAULT_SERVER_SQL="${CMAKE_SOURCE_DIR}/scripts/create_db.sql"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(SageStoreDebugCli PRIVATE -Wall -Wextra -Werror)
endif()
