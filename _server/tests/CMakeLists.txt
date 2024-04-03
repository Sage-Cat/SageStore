find_package(GTest REQUIRED)

set(SERVER_UNIT_TESTS "${SERVER_TESTS_DIR}/unit")
set(SERVER_COMPONENT_TESTS "${SERVER_TESTS_DIR}/component")
set(SERVER_TEST_MOCKS "${SERVER_TESTS_DIR}/mocks")
set(SERVER_TEST_WRAPPERS "${SERVER_TESTS_DIR}/wrappers")

add_subdirectory(component)
add_subdirectory(unit)