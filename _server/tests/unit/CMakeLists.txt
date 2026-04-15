# ------------------------- UNIT -------------------------
set(SERVER_UNIT_TESTS_SOURCE
    ${SERVER_TESTS_DIR}/unit/UsersModuleTest.cpp
    ${BUSINESS_SOURCES}
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${DATABASE_SOURCES}
)

add_executable(UsersModuleTest ${SERVER_UNIT_TESTS_SOURCE})
set_target_properties(UsersModuleTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SERVER_BUILD_DIR}/tests/unit"
)

target_include_directories(UsersModuleTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TESTS_DIR}
)

target_link_libraries(UsersModuleTest 
    PUBLIC
    common
    boost::boost 
    spdlog::spdlog 
    SQLite::SQLite3
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
    GTest::gtest 
    GTest::gtest_main 
    GTest::gmock 
    GTest::gmock_main
)
add_test(NAME Unit_UsersModuleTest COMMAND UsersModuleTest)
message(STATUS "Adding unit test: UsersModuleTest")

# ------------------------- INVENTORY MODULE -------------------------
set(INVENTORY_MODULE_TESTS_SOURCE
    ${SERVER_TESTS_DIR}/unit/InventoryModuleTest.cpp
    ${BUSINESS_SOURCES}
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${DATABASE_SOURCES}
)

add_executable(InventoryModuleTest ${INVENTORY_MODULE_TESTS_SOURCE})
set_target_properties(InventoryModuleTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SERVER_BUILD_DIR}/tests/unit"
)

target_include_directories(InventoryModuleTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TESTS_DIR}
)

target_link_libraries(InventoryModuleTest
    PUBLIC
    common
    boost::boost
    spdlog::spdlog
    SQLite::SQLite3
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
)
add_test(NAME Unit_InventoryModuleTest COMMAND InventoryModuleTest)
message(STATUS "Adding unit test: InventoryModuleTest")

# ------------------------- ERP MODULES -------------------------
set(ERP_MODULE_TESTS_SOURCE
    ${SERVER_TESTS_DIR}/unit/ErpModulesTest.cpp
    ${BUSINESS_SOURCES}
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${DATABASE_SOURCES}
)

add_executable(ErpModulesTest ${ERP_MODULE_TESTS_SOURCE})
set_target_properties(ErpModulesTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SERVER_BUILD_DIR}/tests/unit"
)

target_include_directories(ErpModulesTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TESTS_DIR}
)

target_link_libraries(ErpModulesTest
    PUBLIC
    common
    boost::boost
    spdlog::spdlog
    SQLite::SQLite3
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
)
add_test(NAME Unit_ErpModulesTest COMMAND ErpModulesTest)
message(STATUS "Adding unit test: ErpModulesTest")

# ------------------------- SERVER STARTUP CONFIG -------------------------
set(SERVER_STARTUP_CONFIG_TESTS_SOURCE
    ${SERVER_TESTS_DIR}/unit/ServerStartupConfigTest.cpp
    ${SERVER_SOURCES_DIR}/ServerStartupConfig.cpp
)

add_executable(ServerStartupConfigTest ${SERVER_STARTUP_CONFIG_TESTS_SOURCE})
set_target_properties(ServerStartupConfigTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SERVER_BUILD_DIR}/tests/unit"
)

target_include_directories(ServerStartupConfigTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TESTS_DIR}
)

target_link_libraries(ServerStartupConfigTest
    PUBLIC
    common
    spdlog::spdlog
    GTest::gtest
    GTest::gtest_main
)
add_test(NAME Unit_ServerStartupConfigTest COMMAND ServerStartupConfigTest)
message(STATUS "Adding unit test: ServerStartupConfigTest")

# ------ PRODUCT INFO REPOSITORY TEST ------
set(PRODUCT_INFO_TESTS_SOURCE
    ${SERVER_TESTS_DIR}/unit/ProductInfoRepositoryTest.cpp
    ${BUSINESS_SOURCES}
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${DATABASE_SOURCES}
    ${SERVER_SOURCES_DIR}/Database/ProductInfoRepository.cpp
)

add_executable(ProductInfoRepositoryTest ${PRODUCT_INFO_TESTS_SOURCE})
set_target_properties(ProductInfoRepositoryTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SERVER_BUILD_DIR}/tests/unit"
)

target_include_directories(ProductInfoRepositoryTest PUBLIC
    ${SERVER_TESTS_DIR}
    ${SERVER_SOURCES_DIR}
)

target_link_libraries(ProductInfoRepositoryTest 
    PUBLIC
    common
    boost::boost 
    spdlog::spdlog 
    SQLite::SQLite3
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
    GTest::gtest 
    GTest::gtest_main 
    GTest::gmock 
    GTest::gmock_main
)
add_test(NAME Unit_ProductInfoRepositoryTest COMMAND ProductInfoRepositoryTest)
message(STATUS "Adding unit test: ProductInfoRepositoryTest")
