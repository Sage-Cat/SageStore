set(TEST_FULLSTACK_WORKFLOW_SOURCES
    ${CLIENT_INTEGRATION_TESTS_DIR}/FullstackWorkflowTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(FullstackWorkflowTest ${TEST_FULLSTACK_WORKFLOW_SOURCES})
target_include_directories(FullstackWorkflowTest
    PUBLIC
        ${CLIENT_SOURCES_DIR}
        ${CLIENT_TEST_INCLUDE_DIR}
)
target_link_libraries(FullstackWorkflowTest ${CLIENT_FULLSTACK_TEST_COMMON_LIBS})
target_compile_definitions(FullstackWorkflowTest PRIVATE
    ${CLIENT_FULLSTACK_TEST_DEFINITIONS}
)
message(STATUS "Adding integration test: FullstackWorkflowTest")
add_test(NAME Integration_FullstackWorkflowTest COMMAND FullstackWorkflowTest)
set_tests_properties(
    Integration_FullstackWorkflowTest
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
