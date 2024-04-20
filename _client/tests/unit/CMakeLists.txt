# ------------------------- APIMANAGER -------------------------
set(TEST_API_MANAGER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/ApiManagerTest.cpp
    ${NETWORK_SOURCES}
)
add_executable(ApiManagerTest ${TEST_API_MANAGER_SOURCES})
target_include_directories(ApiManagerTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_MOCKS_DIR})
target_link_libraries(ApiManagerTest 
    common
    Qt6::Test 
    Qt6::Core 
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding unit test: ApiManagerTest")
add_test(NAME Unit_ApiManagerTest COMMAND ApiManagerTest)


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
    common
    Qt6::Test 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding unit test: UiManagerTest")
add_test(NAME Unit_UiManagerTest COMMAND UiManagerTest)


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
    common
    Qt6::Test 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding unit test: DialogManagerTest")
add_test(NAME Unit_DialogManagerTest COMMAND DialogManagerTest)