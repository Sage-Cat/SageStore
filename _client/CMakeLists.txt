project(SageStoreClient)

set(CLIENT_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CLIENT_BUILD_DIR "${CMAKE_BINARY_DIR}/_client")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Locate Qt packages
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Qt6Quick REQUIRED)

# Locate conan packages
find_package(spdlog REQUIRED)

# all src scopes
file(GLOB_RECURSE APPLICATION_LOGIC_SOURCES "${CLIENT_SOURCES_DIR}/ApplicationLogic/*.cpp")
file(GLOB_RECURSE COMMUNICATION_SOURCES "${CLIENT_SOURCES_DIR}/Communication/*.cpp")
file(GLOB_RECURSE UI_SOURCES "${CLIENT_SOURCES_DIR}/UI/*.cpp")
file(GLOB_RECURSE MAIN_SOURCES "${CLIENT_SOURCES_DIR}/*.cpp")

# all the sources
set(ALL_CLIENT_SOURCES ${APPLICATION_LOGIC_SOURCES} ${COMMUNICATION_SOURCES} ${UI_SOURCES} ${MAIN_SOURCES})

# Adding main code and tests
add_subdirectory(src)
add_subdirectory(tests)
