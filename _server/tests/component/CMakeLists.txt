# ------------------------- BUSINESSLOGIC -------------------------
set(TEST_BUSINESS_LOGIC_SOURCES
    ${SERVER_COMPONENT_TEST_DIR}/BusinessLogicTest.cpp
    # Add the source files your test depends on
    ${SERVER_SOURCES}
)

add_executable(BusinessLogicTest ${TEST_BUSINESS_LOGIC_SOURCES})

target_include_directories(BusinessLogicTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_MOCKS_DIR} 
)

target_link_libraries(BusinessLogicTest 
    gtest_main
    gmock
    spdlog::spdlog
)
add_test(NAME Component_BusinessLogicTest COMMAND BusinessLogicTest)
message(STATUS "Adding component test: BusinessLogicTest")