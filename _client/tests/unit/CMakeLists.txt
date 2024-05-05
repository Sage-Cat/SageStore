# ------------------------- APIMANAGER -------------------------
set(TEST_API_MANAGER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/ApiManagerTest.cpp
    ${NETWORK_SOURCES}
)
add_executable(ApiManagerTest ${TEST_API_MANAGER_SOURCES})
target_include_directories(ApiManagerTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(ApiManagerTest 
    common
    Qt6::Test 
    Qt6::Core 
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding unit test: ApiManagerTest")
add_test(NAME Unit_ApiManagerTest COMMAND ApiManagerTest)


# ------------------------- MAINWINDOW -------------------------
set(TEST_UI_MANAGER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/MainWindowTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(MainWindowTest ${TEST_UI_MANAGER_SOURCES} ${CLIENT_TEST_INCLUDE_DIR}/mocks/DialogManagerMock.hpp)
target_include_directories(MainWindowTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(MainWindowTest 
    common
    Qt6::Test 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding unit test: MainWindowTest")
add_test(NAME Unit_MainWindowTest COMMAND MainWindowTest)


# ------------------------- DIALOGMANAGER -------------------------
set(TEST_UI_MANAGER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/DialogManagerTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(DialogManagerTest ${TEST_UI_MANAGER_SOURCES})
target_include_directories(DialogManagerTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
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