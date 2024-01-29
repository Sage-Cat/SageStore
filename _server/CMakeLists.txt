project(SageStoreServer)

# Locate conan packages
find_package(Boost)
find_package(spdlog REQUIRED)

set(SERVER_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(MAIN_SOURCES
    ${SERVER_SOURCES_DIR}/main.cpp
    ${SERVER_SOURCES_DIR}/SageStoreServer.cpp
)

# All server sources
set(ALL_SERVER_SOURCES
    ${MAIN_SOURCES}
)

add_subdirectory(src)

if(BUILD_SERVER_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
