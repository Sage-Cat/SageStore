find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Test)

set(CLIENT_UNIT_TESTS_DIR "${CLIENT_TESTS_DIR}/unit")
set(CLIENT_TEST_MOCKS_DIR "${CLIENT_TESTS_DIR}/mocks")
set(CLIENT_TEST_WRAPPERS_DIR "${CLIENT_TESTS_DIR}/wrappers")

# tests
add_subdirectory(unit)