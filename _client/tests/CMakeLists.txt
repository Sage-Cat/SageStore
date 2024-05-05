find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Test)

set(CLIENT_COMPONENT_TESTS_DIR "${CLIENT_TESTS_DIR}/component")
set(CLIENT_UNIT_TESTS_DIR "${CLIENT_TESTS_DIR}/unit")
set(CLIENT_TEST_INCLUDE_DIR "${CLIENT_TESTS_DIR}/include")

# tests
add_subdirectory(unit)
add_subdirectory(component)