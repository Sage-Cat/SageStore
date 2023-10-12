cmake_minimum_required(VERSION 3.15)

project(SageStoreServer)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Specify source files
file(GLOB_RECURSE BUSINESS_LOGIC_SOURCES "src/BusinessLogic/*.cpp")
file(GLOB_RECURSE DATAACCESSLAYER_SOURCES "src/DataAccessLayer/*.cpp")
file(GLOB_RECURSE IMEDIATOR "src/IMediator/*.cpp")  

set(CLIENT_SOURCES
    src/main.cpp
    src/SageStoreClient.cpp
    ${BUSINESS_LOGIC_SOURCES}
    ${DATAACCESSLAYER_SOURCES}
    ${IMEDIATOR}
)

# Create server executable
add_executable(SageStoreServer ${SERVER_SOURCES})

# Include directories
target_include_directories(SageStoreServer PUBLIC ${CMAKE_SOURCE_DIR}/Server/src)
