set(TEST_FULLSTACK_INVENTORY_WORKFLOW_SOURCES
    ${CLIENT_SYSTEM_TESTS_DIR}/FullstackInventoryWorkflowTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(FullstackInventoryWorkflowTest ${TEST_FULLSTACK_INVENTORY_WORKFLOW_SOURCES})
target_include_directories(FullstackInventoryWorkflowTest
    PUBLIC
        ${CLIENT_SOURCES_DIR}
        ${CLIENT_TEST_INCLUDE_DIR}
)
target_link_libraries(FullstackInventoryWorkflowTest ${CLIENT_FULLSTACK_TEST_COMMON_LIBS})
target_compile_definitions(FullstackInventoryWorkflowTest PRIVATE
    ${CLIENT_FULLSTACK_TEST_DEFINITIONS}
)
message(STATUS "Adding system test: FullstackInventoryWorkflowTest")
add_test(NAME System_FullstackInventoryWorkflowTest COMMAND FullstackInventoryWorkflowTest)
set_tests_properties(
    System_FullstackInventoryWorkflowTest
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
