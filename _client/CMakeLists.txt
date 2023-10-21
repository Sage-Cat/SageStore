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
find_package(Qt6 COMPONENTS QuickControls2 REQUIRED)


# Locate conan packages
find_package(spdlog REQUIRED)

# Define source files
set(APPLICATION_LOGIC_SOURCES
    # Empty for now
)

set(COMMUNICATION_SOURCES
    # Empty for now
)

set(UI_SOURCES
    ${CLIENT_SOURCES_DIR}/Ui/QmlTypeRegistrar.cpp
    ${CLIENT_SOURCES_DIR}/ViewModels/PurchaseOrdersViewModel.cpp
)

set(MAIN_SOURCES
    ${CLIENT_SOURCES_DIR}/logging.cpp
    ${CLIENT_SOURCES_DIR}/UiManager.cpp
    ${CLIENT_SOURCES_DIR}/SageStoreClient.cpp
    ${CLIENT_SOURCES_DIR}/main.cpp
)

# All sources
set(ALL_CLIENT_SOURCES
    ${APPLICATION_LOGIC_SOURCES}
    ${COMMUNICATION_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)

# Adding main code and tests
add_subdirectory(src)
add_subdirectory(tests)
