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

# ------------------------- STARTUPCONTROLLER -------------------------
set(TEST_STARTUP_CONTROLLER_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/StartupControllerTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(StartupControllerTest ${TEST_STARTUP_CONTROLLER_SOURCES}
    ${CLIENT_TEST_INCLUDE_DIR}/mocks/DialogManagerMock.hpp)
target_include_directories(StartupControllerTest PUBLIC ${CLIENT_SOURCES_DIR}
    ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(StartupControllerTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: StartupControllerTest")
add_test(NAME Unit_StartupControllerTest COMMAND StartupControllerTest)


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

set(TEST_USERS_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/UsersViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(UsersViewTest ${TEST_USERS_VIEW_SOURCES})
target_include_directories(UsersViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(UsersViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: UsersViewTest")
add_test(NAME Unit_UsersViewTest COMMAND UsersViewTest)

set(TEST_ROLES_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/RolesViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(RolesViewTest ${TEST_ROLES_VIEW_SOURCES})
target_include_directories(RolesViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(RolesViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: RolesViewTest")
add_test(NAME Unit_RolesViewTest COMMAND RolesViewTest)

set(TEST_MANAGEMENT_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/ManagementViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(ManagementViewTest ${TEST_MANAGEMENT_VIEW_SOURCES})
target_include_directories(ManagementViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(ManagementViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: ManagementViewTest")
add_test(NAME Unit_ManagementViewTest COMMAND ManagementViewTest)

set(TEST_SUPPLIER_CATALOG_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/SupplierCatalogViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(SupplierCatalogViewTest ${TEST_SUPPLIER_CATALOG_VIEW_SOURCES})
target_include_directories(SupplierCatalogViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(SupplierCatalogViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: SupplierCatalogViewTest")
add_test(NAME Unit_SupplierCatalogViewTest COMMAND SupplierCatalogViewTest)

set(TEST_SETTINGS_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/SettingsViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(SettingsViewTest ${TEST_SETTINGS_VIEW_SOURCES})
target_include_directories(SettingsViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(SettingsViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: SettingsViewTest")
add_test(NAME Unit_SettingsViewTest COMMAND SettingsViewTest)

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

set(TEST_SALES_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/SalesViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(SalesViewTest ${TEST_SALES_VIEW_SOURCES})
target_include_directories(SalesViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(SalesViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: SalesViewTest")
add_test(NAME Unit_SalesViewTest COMMAND SalesViewTest)

set(TEST_PURCHASE_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/PurchaseViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(PurchaseViewTest ${TEST_PURCHASE_VIEW_SOURCES})
target_include_directories(PurchaseViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(PurchaseViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: PurchaseViewTest")
add_test(NAME Unit_PurchaseViewTest COMMAND PurchaseViewTest)

set(TEST_ANALYTICS_VIEW_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/AnalyticsViewTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(AnalyticsViewTest ${TEST_ANALYTICS_VIEW_SOURCES})
target_include_directories(AnalyticsViewTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(AnalyticsViewTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: AnalyticsViewTest")
add_test(NAME Unit_AnalyticsViewTest COMMAND AnalyticsViewTest)

set(TEST_ERP_MODELS_SOURCES
    ${CLIENT_UNIT_TESTS_DIR}/ErpModelsTest.cpp
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)
add_executable(ErpModelsTest ${TEST_ERP_MODELS_SOURCES})
target_include_directories(ErpModelsTest PUBLIC ${CLIENT_SOURCES_DIR} ${CLIENT_TEST_INCLUDE_DIR})
target_link_libraries(ErpModelsTest
    common
    Qt6::Test
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    spdlog::spdlog
)
message(STATUS "Adding unit test: ErpModelsTest")
add_test(NAME Unit_ErpModelsTest COMMAND ErpModelsTest)

set_tests_properties(
    Unit_ApiManagerTest
    Unit_MainWindowTest
    Unit_StartupControllerTest
    Unit_DialogManagerTest
    Unit_UsersManagementModelTest
    Unit_ProductTypesModelTest
    Unit_ProductTypesViewTest
    Unit_UsersViewTest
    Unit_RolesViewTest
    Unit_ManagementViewTest
    Unit_SupplierCatalogViewTest
    Unit_SettingsViewTest
    Unit_StocksModelTest
    Unit_StocksViewTest
    Unit_SalesViewTest
    Unit_PurchaseViewTest
    Unit_AnalyticsViewTest
    Unit_ErpModelsTest
    PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
