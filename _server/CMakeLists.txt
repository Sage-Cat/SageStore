project(${SERVER_NAME})

# Locate conan packages

find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(spdlog REQUIRED)

# Find Boost potentially provided by Conan or manually
find_package(Boost QUIET) # QUIET to not fail if it doesn't find it here.

# Check if the Conan-provided target exists and alias it if it does
if(TARGET Boost::boost)
    # Alias Boost::boost to a common name we will use
    add_library(boost_common ALIAS Boost::boost)
elseif(TARGET boost::boost)
    # Alias boost::boost to the same common name
    add_library(boost_common ALIAS boost::boost)
else()
    # Error or fallback handling if neither target exists
    message(FATAL_ERROR "Boost not found")
endif()

set(SERVER_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(SERVER_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(SERVER_BUILD_DIR "${CMAKE_BINARY_DIR}/_server")

set(BUSINESS_SOURCES
    ${SERVER_SOURCES_DIR}/BusinessLogic/BusinessLogic.cpp 

    # Modules
    ${SERVER_SOURCES_DIR}/BusinessLogic/UsersModule.cpp
)

set(DATABASE_SOURCES
    ${SERVER_SOURCES_DIR}/Database/RepositoryManager.cpp
    ${SERVER_SOURCES_DIR}/Database/SqliteDatabaseManager.cpp

    # Repositories
    ${SERVER_SOURCES_DIR}/Database/UsersRepository.cpp
    ${SERVER_SOURCES_DIR}/Database/RolesRepository.cpp
)

set(NETWORK_SOURCES
    ${SERVER_SOURCES_DIR}/Network/HttpServer.cpp 
    ${SERVER_SOURCES_DIR}/Network/HttpTransaction.cpp 
    ${SERVER_SOURCES_DIR}/Network/JsonSerializer.cpp
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
