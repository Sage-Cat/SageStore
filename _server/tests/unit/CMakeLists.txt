find_package(GTest REQUIRED)

# Define test sources
file(GLOB TEST_SOURCES "*.cpp")

# Create test executables
foreach(testSrc ${TEST_SOURCES})
    get_filename_component(testName ${testSrc} NAME_WE)
    set(testExecutable "${testName}")

    add_executable(${testExecutable} ${testSrc})

    target_include_directories(${testExecutable} PUBLIC ${SERVER_SOURCES_DIR})

    target_link_libraries(${testExecutable} ServerTestLib)
    target_link_libraries(${testExecutable} GTest::GTest GTest::Main)
    target_link_libraries(${testExecutable} spdlog::spdlog)

    add_test(NAME ${testName} COMMAND ${testExecutable})
endforeach()