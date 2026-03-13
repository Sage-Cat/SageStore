# ------------------------- UNIT -------------------------
set(SERVER_UNIT_TESTS_SOURCE
    ${SERVER_TESTS_DIR}/unit/UsersModuleTest.cpp
    ${BUSINESS_SOURCES}
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/ProductTypeRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/UserRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RoleRepository.cpp
)

add_executable(UsersModuleTest ${SERVER_UNIT_TESTS_SOURCE})

target_include_directories(UsersModuleTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TESTS_DIR}
)

target_link_libraries(UsersModuleTest 
    PUBLIC
    common
    boost::boost 
    spdlog::spdlog 
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
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/ProductTypeRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/UserRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RoleRepository.cpp
)

add_executable(InventoryModuleTest ${INVENTORY_MODULE_TESTS_SOURCE})

target_include_directories(InventoryModuleTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TESTS_DIR}
)

target_link_libraries(InventoryModuleTest
    PUBLIC
    common
    boost::boost
    spdlog::spdlog
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
)
add_test(NAME Unit_InventoryModuleTest COMMAND InventoryModuleTest)
message(STATUS "Adding unit test: InventoryModuleTest")

# ------ PRODUCT INFO REPOSITORY TEST ------
set(PRODUCT_INFO_TESTS_SOURCE
    ${SERVER_TESTS_DIR}/unit/ProductInfoRepositoryTest.cpp
    ${BUSINESS_SOURCES}
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/ProductInfoRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/ProductTypeRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/UserRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RoleRepository.cpp
)

add_executable(ProductInfoRepositoryTest ${PRODUCT_INFO_TESTS_SOURCE})

target_include_directories(ProductInfoRepositoryTest PUBLIC
    ${PRODUCT_INFO_TESTS_SOURCE}
    ${SERVER_TESTS_DIR}
    ${SERVER_SOURCES_DIR}
)

target_link_libraries(ProductInfoRepositoryTest 
    PUBLIC
    common
    boost::boost 
    spdlog::spdlog 
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
    GTest::gtest 
    GTest::gtest_main 
    GTest::gmock 
    GTest::gmock_main
)
add_test(NAME Unit_ProductInfoRepositoryTest COMMAND ProductInfoRepositoryTest)
message(STATUS "Adding unit test: ProductInfoRepositoryTest")

