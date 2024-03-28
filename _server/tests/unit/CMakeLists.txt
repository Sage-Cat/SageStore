# ------------------------- UNIT -------------------------
set(SERVER_UNIT_TESTS_DIR
    ${SERVER_TESTS_DIR}/unit/UserModuleTest.cpp
    ${BUSINESS_SOURCES}
    ${SERVER_SOURCES_DIR}/ServerException.cpp
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/UsersRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RolesRepository.cpp
)

add_executable(UsersModuleTest ${SERVER_UNIT_TESTS_DIR})

target_include_directories(UsersModuleTest PUBLIC
    ${SERVER_SOURCES_DIR}
    ${SERVER_TEST_MOCKS_DIR} 
)

target_link_libraries(UsersModuleTest 
    PUBLIC
    boost::boost 
    spdlog::spdlog 
    $<$<PLATFORM_ID:Windows>:Ws2_32>
    $<$<PLATFORM_ID:Windows>:Wsock32>
    GTest::gtest 
    GTest::gtest_main 
    GTest::gmock 
    GTest::gmock_main
)
add_test(NAME UsersModuleTest COMMAND UsersModuleTest)
message(STATUS "Adding unit test: UsersModuleTest")