project(SageStoreServer)

set(SERVER_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Locate conan packages
find_package(spdlog REQUIRED)

# Specify source files
set(SERVER_SOURCES
    src/main.cpp
    src/SageStoreServer.cpp
    ${BUSINESS_LOGIC_SOURCES}
    ${DATA_ACCESS_LAYER_SOURCES}
    ${IMEDIATOR}
)

# Create server executable
add_executable(SageStoreServer ${SERVER_SOURCES} ${COMMON_SOURCES})

# Include directories
target_include_directories(SageStoreServer PUBLIC ${SERVER_SOURCES_DIR})

# link libs
target_link_libraries(SageStoreServer spdlog::spdlog)
