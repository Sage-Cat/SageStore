# Create a static library from the Client source files
add_library(ClientTestLib STATIC ${ALL_CLIENT_SOURCES})
target_include_directories(ClientTestLib PUBLIC ${CLIENT_SOURCES_DIR})
target_link_libraries(ClientTestLib Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Quick spdlog::spdlog)

find_package(GTest REQUIRED)

# Define test sources
file(GLOB TEST_SOURCES "*.cpp")

# Create test executables
foreach(testSrc ${TEST_SOURCES})
    get_filename_component(testName ${testSrc} NAME_WE)
    set(testExecutable "${testName}")

    add_executable(${testExecutable} ${testSrc} ${COMMON_SOURCES})

    target_include_directories(${testExecutable} PUBLIC ${CLIENT_SOURCES_DIR})

    target_link_libraries(${testExecutable} ClientTestLib)
    target_link_libraries(${testExecutable} GTest::GTest GTest::Main)
    target_link_libraries(${testExecutable} Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Quick spdlog::spdlog)

    add_test(NAME ${testName} COMMAND ${testExecutable})
endforeach()