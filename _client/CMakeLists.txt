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

set(NETWORK_SOURCES
    ${CLIENT_SOURCES_DIR}/Network/ConfigManager.cpp
    ${CLIENT_SOURCES_DIR}/Network/NetworkService.cpp
    ${CLIENT_SOURCES_DIR}/Network/JsonSerializer.cpp
)

set(UI_SOURCES
    # Models
    ${CLIENT_SOURCES_DIR}/Models/AuthorizationModel.cpp
    ${CLIENT_SOURCES_DIR}/Models/RegistrationModel.cpp

    # ViewModels
    ${CLIENT_SOURCES_DIR}/ViewModels/PurchaseOrdersViewModel.cpp
    ${CLIENT_SOURCES_DIR}/ViewModels/AuthorizationViewModel.cpp
    ${CLIENT_SOURCES_DIR}/ViewModels/RegistrationViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Views/ViewStyles.hpp
    ${CLIENT_SOURCES_DIR}/Views/MainWindowView.cpp
    ${CLIENT_SOURCES_DIR}/Views/BaseView.cpp  
    ${CLIENT_SOURCES_DIR}/Views/PurchaseOrdersView.cpp
)

set(MAIN_SOURCES
    ${CLIENT_SOURCES_DIR}/main.cpp
    ${CLIENT_SOURCES_DIR}/UiManager.cpp
    ${CLIENT_SOURCES_DIR}/SageStoreClient.cpp
    ${CLIENT_SOURCES_DIR}/SpdlogWrapper.cpp
)

# All sources
set(ALL_CLIENT_SOURCES
    ${APPLICATION_LOGIC_SOURCES}
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
)

# Adding main code and tests
add_subdirectory(src)
add_subdirectory(tests)
