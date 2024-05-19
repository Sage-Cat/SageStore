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