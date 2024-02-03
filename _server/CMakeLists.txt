project(SageStoreServer)

# Locate conan packages
find_package(Boost)
# find_package(wolfssl)
find_package(spdlog REQUIRED)
find_package(nlohmann_json)

set(SERVER_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SERVER_BUILD_DIR "${CMAKE_BINARY_DIR}/_server")

set(BUSINESS_SOURCESS
    ${SERVER_SOURCES_DIR}/BusinessLogic/BusinessLogicFacade.cpp 
    ${SERVER_SOURCES_DIR}/BusinessLogic/UsersModule.cpp
)

set(NETWORK_SOURCES
    ${SERVER_SOURCES_DIR}/Network/HttpServer.cpp 
    ${SERVER_SOURCES_DIR}/Network/HttpSession.cpp 
    ${SERVER_SOURCES_DIR}/Network/JsonSerializer.cpp
    
    ${SERVER_SOURCES_DIR}/Network/NetworkCommon.hpp
)

set(MAIN_SOURCES
    ${SERVER_SOURCES_DIR}/main.cpp
    ${SERVER_SOURCES_DIR}/SageStoreServer.cpp
)

# All server sources
set(ALL_SERVER_SOURCES
    ${BUSINESS_SOURCESS}
    ${NETWORK_SOURCES}
    ${MAIN_SOURCES}
)

add_subdirectory(src)

if(BUILD_SERVER_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
