set(TEST_FULLSTACK_MAINWINDOW_SOURCES
    ${CLIENT_COMPONENT_TESTS_DIR}/FullstackMainWindowWorkflowTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(FullstackMainWindowWorkflowTest ${TEST_FULLSTACK_MAINWINDOW_SOURCES})
target_include_directories(FullstackMainWindowWorkflowTest
    PUBLIC
        ${CLIENT_SOURCES_DIR}
        ${CLIENT_TEST_INCLUDE_DIR}
)
target_link_libraries(FullstackMainWindowWorkflowTest ${CLIENT_FULLSTACK_TEST_COMMON_LIBS})
target_compile_definitions(FullstackMainWindowWorkflowTest PRIVATE
    ${CLIENT_FULLSTACK_TEST_DEFINITIONS}
)
message(STATUS "Adding component test: FullstackMainWindowWorkflowTest")
add_test(NAME Component_FullstackMainWindowWorkflowTest COMMAND FullstackMainWindowWorkflowTest)
set_tests_properties(
    Component_FullstackMainWindowWorkflowTest
    PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        LABELS "client;ui;fullstack;component;offscreen_regression"
        TIMEOUT 120
        RESOURCE_LOCK "SageStoreUiOffscreen"
)

set(TEST_FULLSTACK_SALES_EXPORT_SOURCES
    ${CLIENT_COMPONENT_TESTS_DIR}/FullstackSalesInvoiceExportTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(FullstackSalesInvoiceExportTest ${TEST_FULLSTACK_SALES_EXPORT_SOURCES})
target_include_directories(FullstackSalesInvoiceExportTest
    PUBLIC
        ${CLIENT_SOURCES_DIR}
        ${CLIENT_TEST_INCLUDE_DIR}
)
target_link_libraries(FullstackSalesInvoiceExportTest ${CLIENT_FULLSTACK_TEST_COMMON_LIBS})
target_compile_definitions(FullstackSalesInvoiceExportTest PRIVATE
    ${CLIENT_FULLSTACK_TEST_DEFINITIONS}
)
message(STATUS "Adding component test: FullstackSalesInvoiceExportTest")
add_test(NAME Component_FullstackSalesInvoiceExportTest COMMAND FullstackSalesInvoiceExportTest)
set_tests_properties(
    Component_FullstackSalesInvoiceExportTest
    PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        LABELS "client;ui;fullstack;component;offscreen_regression"
        TIMEOUT 120
        RESOURCE_LOCK "SageStoreUiOffscreen"
)

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
    PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        LABELS "client;ui;fullstack;integration;offscreen_regression"
        TIMEOUT 120
        RESOURCE_LOCK "SageStoreUiOffscreen"
)

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
    PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        LABELS "client;ui;fullstack;system;offscreen_regression"
        TIMEOUT 120
        RESOURCE_LOCK "SageStoreUiOffscreen"
)

set(TEST_DEBUG_CLI_SCENARIO_SOURCES
    ${CLIENT_SYSTEM_TESTS_DIR}/DebugCliScenarioTest.cpp
)
add_executable(DebugCliScenarioTest ${TEST_DEBUG_CLI_SCENARIO_SOURCES})
target_include_directories(DebugCliScenarioTest
    PUBLIC
        ${CLIENT_SOURCES_DIR}
        ${CLIENT_TEST_INCLUDE_DIR}
)
target_link_libraries(DebugCliScenarioTest ${CLIENT_FULLSTACK_TEST_COMMON_LIBS})
target_compile_definitions(DebugCliScenarioTest PRIVATE
    ${CLIENT_FULLSTACK_TEST_DEFINITIONS}
)
message(STATUS "Adding system test: DebugCliScenarioTest")
add_test(NAME System_DebugCliScenarioTest COMMAND DebugCliScenarioTest)
set_tests_properties(
    System_DebugCliScenarioTest
    PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
        LABELS "client;ui;fullstack;system;debugcli;offscreen_regression"
        TIMEOUT 180
        RESOURCE_LOCK "SageStoreUiOffscreen"
)
