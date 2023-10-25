project(SageStoreServer)

# Locate conan packages
find_package(spdlog REQUIRED)

# Specify source files
file(GLOB_RECURSE API_SOURCES "src/API_Endpoints/*.cpp")  
file(GLOB_RECURSE BUSINESS_LOGIC_SOURCES "src/BusinessLogic/*.cpp")
file(GLOB_RECURSE DATA_ACCESS_SOURCES "src/DataAccess/*.cpp")
file(GLOB_RECURSE MEDIATORS_SOURCES "src/Mediators/*.cpp")  

set(SERVER_SOURCES
    src/main.cpp
    src/SageStoreServer.cpp
    ${BUSINESS_LOGIC_SOURCES}
    ${DATA_ACCESS_LAYER_SOURCES}
    ${IMEDIATOR}
)

# Create server executable
add_executable(SageStoreServer ${SERVER_SOURCES})

# Include directories
target_include_directories(SageStoreServer PUBLIC ${CMAKE_SOURCE_DIR}/_server/src)

# link libs
target_link_libraries(SageStoreServer spdlog::spdlog)
