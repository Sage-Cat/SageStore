find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Test)

# ------------------------- APIMANAGER -------------------------
set(TEST_API_MANAGER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/ApiManagerTest.cpp
    ${NETWORK_SOURCES}
)
add_executable(ApiManagerTest ${TEST_API_MANAGER_SOURCES})
target_include_directories(ApiManagerTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_MOCKS_DIR})
target_link_libraries(ApiManagerTest 
    Qt6::Test 
    Qt6::Core 
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding test: ApiManagerTest")
add_test(NAME ApiManagerTest COMMAND ApiManagerTest)


# ------------------------- UIMANAGER -------------------------
set(TEST_UI_MANAGER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/UiManagerTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(UiManagerTest ${TEST_UI_MANAGER_SOURCES})
target_include_directories(UiManagerTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_MOCKS_DIR} ${CLIENT_TEST_WRAPPERS_DIR})
target_link_libraries(UiManagerTest 
    Qt6::Test 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding test: UiManagerTest")
add_test(NAME UiManagerTest COMMAND UiManagerTest)


# ------------------------- DIALOGMANAGER -------------------------
set(TEST_UI_MANAGER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/DialogManagerTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(DialogManagerTest ${TEST_UI_MANAGER_SOURCES})
target_include_directories(DialogManagerTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_MOCKS_DIR} ${CLIENT_TEST_WRAPPERS_DIR})
target_link_libraries(DialogManagerTest 
    Qt6::Test 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding test: DialogManagerTest")
add_test(NAME DialogManagerTest COMMAND DialogManagerTest)