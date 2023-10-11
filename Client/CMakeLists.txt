cmake_minimum_required(VERSION 3.23)

project(SageStoreClient)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Handle the rcc files automatically
set(CMAKE_AUTORCC ON)

# Locate the QtWidgets package
find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)
find_package(Qt6Gui REQUIRED)
find_package(Qt6Quick REQUIRED)

# Locate conan packages
find_package(spdlog REQUIRED)

# Specify source files
file(GLOB_RECURSE APPLICATION_LOGIC_SOURCES "src/ApplicationLogic/*.cpp")
file(GLOB_RECURSE COMMUNICATION_SOURCES "src/Communication/*.cpp")
file(GLOB_RECURSE UI_SOURCES "src/UI/*.cpp")  

# Specify resource files
set(RESOURCE_FILES "src/UI/qml/qml.qrc")

set(CLIENT_SOURCES
    src/main.cpp
    src/SageStoreClient.cpp
    ${APPLICATION_LOGIC_SOURCES}
    ${COMMUNICATION_SOURCES}
    ${UI_SOURCES}
)

# Create client executable
add_executable(SageStoreClient ${CLIENT_SOURCES} ${RESOURCE_FILES})

# Include directories
target_include_directories(SageStoreClient PUBLIC ${CMAKE_SOURCE_DIR}/Client/src)

# Link libraries
target_link_libraries(SageStoreClient Qt6::Core  Qt6::Widgets Qt6::Gui Qt6::Quick spdlog::spdlog)


