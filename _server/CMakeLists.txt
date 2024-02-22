project(${SERVER_NAME})

# Locate conan packages
find_package(Boost)
find_package(spdlog REQUIRED)
find_package(nlohmann_json)
find_package(SQLite3)

set(SERVER_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SERVER_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(SERVER_BUILD_DIR "${CMAKE_BINARY_DIR}/_server")

set(BUSINESS_SOURCESS
    ${SERVER_SOURCES_DIR}/BusinessLogic/BusinessLogic.cpp 

    # Modules
    ${SERVER_SOURCES_DIR}/BusinessLogic/UsersModule.cpp
)

set(DATABASE_SOURCES
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/DatabaseManager.cpp
    ${SERVER_SOURCES_DIR}/Database/UsersRepository.cpp

    # Repositories
    ${SERVER_SOURCES_DIR}/Database/UsersRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RolesRepository.cpp

    # Entities
    ${SERVER_SOURCES_DIR}/Database/Entities/User.hpp
    ${SERVER_SOURCES_DIR}/Database/Entities/Role.hpp
)

set(NETWORK_SOURCES
    ${SERVER_SOURCES_DIR}/Network/HttpServer.cpp 
    ${SERVER_SOURCES_DIR}/Network/HttpTransaction.cpp 
    ${SERVER_SOURCES_DIR}/Network/JsonSerializer.cpp
    
    ${SERVER_SOURCES_DIR}/Network/NetworkCommon.hpp
)

set(MAIN_SOURCES
    ${SERVER_SOURCES_DIR}/main.cpp
    ${SERVER_SOURCES_DIR}/SageStoreServer.cpp
    ${SERVER_SOURCES_DIR}/ServerException.cpp
)

# All server sources
set(ALL_SERVER_SOURCES
    ${BUSINESS_SOURCESS}
    ${DATABASE_SOURCES}
    ${NETWORK_SOURCES}
    ${MAIN_SOURCES}
)

if (BUILD_SERVER)
    add_subdirectory(src)
endif()

if(BUILD_TESTS)
    # add_subdirectory(tests)
endif()
