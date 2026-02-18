# ------------------------- BUSINESSLOGIC -------------------------
set(TEST_BUSINESS_LOGIC_SOURCES
    ${SERVER_TESTS_DIR}/component/BusinessLogicTest.cpp
    ${BUSINESS_SOURCES}
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/UserRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RoleRepository.cpp
)

add_executable(BusinessLogicTest ${TEST_BUSINESS_LOGIC_SOURCES})

target_include_directories(BusinessLogicTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TESTS_DIR}
)

target_link_libraries(BusinessLogicTest 
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
add_test(NAME Component_BusinessLogicTest COMMAND BusinessLogicTest)
message(STATUS "Adding component test: BusinessLogicTest")


# ------------------------- REPOSITORY INTEGRATION -------------------------
set(TEST_REPOSITORY_INTEGRATION_SOURCES
    ${SERVER_TESTS_DIR}/component/RepositoryIntegrationTest.cpp
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/SqliteDatabaseManager.cpp
    ${SERVER_SOURCES_DIR}/Database/UserRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RoleRepository.cpp
)

add_executable(RepositoryIntegrationTest ${TEST_REPOSITORY_INTEGRATION_SOURCES})

target_include_directories(RepositoryIntegrationTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TESTS_DIR}
)

target_link_libraries(RepositoryIntegrationTest
    PUBLIC
    common
    boost::boost
    spdlog::spdlog
    SQLite::SQLite3
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
    GTest::gtest
    GTest::gtest_main
)

target_compile_definitions(RepositoryIntegrationTest PRIVATE
    TEST_CREATE_DB_SQL_FILE_PATH="${CMAKE_SOURCE_DIR}/scripts/create_db.sql"
)

add_test(NAME Component_RepositoryIntegrationTest COMMAND RepositoryIntegrationTest)
message(STATUS "Adding component test: RepositoryIntegrationTest")
