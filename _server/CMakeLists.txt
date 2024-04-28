project(${SERVER_NAME})

# Locate conan packages

find_package(SQLite3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost QUIET)

# Check if the Conan-provided
if(TARGET Boost::boost)
    add_library(boost_common ALIAS Boost::boost)
elseif(TARGET boost::boost)
    add_library(boost_common ALIAS boost::boost)
else()
    message(FATAL_ERROR "Boost not found")
endif()

set(SERVER_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SERVER_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(SERVER_BUILD_DIR ${CMAKE_BINARY_DIR}/_server)

set(BUSINESS_SOURCES
    ${SERVER_SOURCES_DIR}/BusinessLogic/BusinessLogic.cpp 

    # Modules
    ${SERVER_SOURCES_DIR}/BusinessLogic/UsersModule.cpp
)

set(DATABASE_SOURCES
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/SqliteDatabaseManager.cpp

    # Repositories
    ${SERVER_SOURCES_DIR}/Database/UserRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RoleRepository.cpp
)

set(NETWORK_SOURCES
    ${SERVER_SOURCES_DIR}/Network/HttpServer.cpp 
    ${SERVER_SOURCES_DIR}/Network/HttpTransaction.cpp 
)

set(MAIN_SOURCES
    ${SERVER_SOURCES_DIR}/SageStoreServer.cpp
    ${SERVER_SOURCES_DIR}/ServerException.cpp
)

# All server sources
set(ALL_SERVER_SOURCES
    ${BUSINESS_SOURCES}
    ${DATABASE_SOURCES}
    ${NETWORK_SOURCES}
    ${MAIN_SOURCES}
    ${SERVER_SOURCES_DIR}/main.cpp
)

if (BUILD_SERVER)
    add_subdirectory(src)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
