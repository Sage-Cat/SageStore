find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Test)

set(CLIENT_COMPONENT_TESTS_DIR "${CLIENT_TESTS_DIR}/component")
set(CLIENT_INTEGRATION_TESTS_DIR "${CLIENT_TESTS_DIR}/integration")
set(CLIENT_SYSTEM_TESTS_DIR "${CLIENT_TESTS_DIR}/system")
set(CLIENT_UNIT_TESTS_DIR "${CLIENT_TESTS_DIR}/unit")
set(CLIENT_TEST_INCLUDE_DIR "${CLIENT_TESTS_DIR}/include")

set(CLIENT_FULLSTACK_TEST_COMMON_LIBS
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)

set(CLIENT_FULLSTACK_TEST_DEFINITIONS
    SAGESTORE_TEST_SERVER_BIN="${CMAKE_BINARY_DIR}/_server/${SERVER_NAME}${CMAKE_EXECUTABLE_SUFFIX}"
    SAGESTORE_TEST_SERVER_SQL="${CMAKE_SOURCE_DIR}/scripts/create_db.sql"
    SAGESTORE_DEBUGCLI_BIN="${CMAKE_BINARY_DIR}/_client/SageStoreDebugCli${CMAKE_EXECUTABLE_SUFFIX}"
)

# tests
if(EXISTS "${CLIENT_COMPONENT_TESTS_DIR}/CMakeLists.txt")
    add_subdirectory(component)
endif()

add_subdirectory(unit)
