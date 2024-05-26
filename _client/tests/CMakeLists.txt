find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Test)

set(CLIENT_COMPONENT_TESTS_DIR "${CLIENT_TESTS_DIR}/component")
set(CLIENT_UNIT_TESTS_DIR "${CLIENT_TESTS_DIR}/unit")
set(CLIENT_TEST_INCLUDE_DIR "${CLIENT_TESTS_DIR}/include")

# tests
add_subdirectory(unit)

# Component tests are not applicable for the UI part 
# and for the Networking part it's enough to have unit tests
# TODO:
#add_subdirectory(component)