# ------------------------- UIMANAGER -------------------------
set(TEST_UI_MANAGER_SOURCES
    ${CLIENT_COMPONENT_TESTS_DIR}/UiTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(UiTest ${TEST_UI_MANAGER_SOURCES})
target_include_directories(UiTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR} )
target_link_libraries(UiTest 
    common
    Qt6::Test 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding component test: MainWindowTest")
add_test(NAME Component_UiTest COMMAND UiTest)
