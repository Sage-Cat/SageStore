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
set(TEST_MAINWINDOW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/MainWindowTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(MainWindowTest ${TEST_MAINWINDOW_SOURCES} ${CLIENT_TEST_INCLUDE_DIR}/mocks/DialogManagerMock.hpp)
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
set(TEST_DIALOGMANAGER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/DialogManagerTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(DialogManagerTest ${TEST_DIALOGMANAGER_SOURCES})
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

# ------------------------- USERMANAGEMENTMODEL -------------------------
set(TEST_USERMANAGEMENTMODEL_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/UsersManagementModelTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(UsersManagementModelTest ${TEST_USERMANAGEMENTMODEL_SOURCES})
target_include_directories(UsersManagementModelTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(UsersManagementModelTest 
    common
    Qt6::Test 
    Qt6::Core 
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network 
    spdlog::spdlog
)
message(STATUS "Adding unit test: UsersManagementModelTest")
add_test(NAME Unit_UsersManagementModelTest COMMAND UsersManagementModelTest)

set(TEST_PRODUCT_TYPES_MODEL_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/ProductTypesModelTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(ProductTypesModelTest ${TEST_PRODUCT_TYPES_MODEL_SOURCES})
target_include_directories(ProductTypesModelTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(ProductTypesModelTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: ProductTypesModelTest")
add_test(NAME Unit_ProductTypesModelTest COMMAND ProductTypesModelTest)

set(TEST_PRODUCT_TYPES_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/ProductTypesViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(ProductTypesViewTest ${TEST_PRODUCT_TYPES_VIEW_SOURCES})
target_include_directories(ProductTypesViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(ProductTypesViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: ProductTypesViewTest")
add_test(NAME Unit_ProductTypesViewTest COMMAND ProductTypesViewTest)

set(TEST_STOCKS_MODEL_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/StocksModelTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(StocksModelTest ${TEST_STOCKS_MODEL_SOURCES})
target_include_directories(StocksModelTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(StocksModelTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: StocksModelTest")
add_test(NAME Unit_StocksModelTest COMMAND StocksModelTest)

set(TEST_STOCKS_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/StocksViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(StocksViewTest ${TEST_STOCKS_VIEW_SOURCES})
target_include_directories(StocksViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(StocksViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: StocksViewTest")
add_test(NAME Unit_StocksViewTest COMMAND StocksViewTest)

set_tests_properties(
    Unit_ApiManagerTest
    Unit_MainWindowTest
    Unit_DialogManagerTest
    Unit_UsersManagementModelTest
    Unit_ProductTypesModelTest
    Unit_ProductTypesViewTest
    Unit_StocksModelTest
    Unit_StocksViewTest
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
