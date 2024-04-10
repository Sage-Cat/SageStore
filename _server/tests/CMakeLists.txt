find_package(GTest REQUIRED)

set(SERVER_UNIT_TESTS_DIR "${SERVER_TESTS_DIR}/unit")
set(SERVER_COMPONENT_TESTS_DIR "${SERVER_TESTS_DIR}/component")
set(SERVER_TEST_MOCKS_DIR "${SERVER_TESTS_DIR}/mocks")
set(SERVER_TEST_WRAPPERS_DIR "${SERVER_TESTS_DIR}/wrappers")

add_subdirectory(component)
add_subdirectory(unit)