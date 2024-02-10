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
    ${CLIENT_SOURCES_DIR}/Network/ApiConstants.hpp
    ${CLIENT_SOURCES_DIR}/Network/ApiManager.cpp
    ${CLIENT_SOURCES_DIR}/Network/NetworkService.cpp
    ${CLIENT_SOURCES_DIR}/Network/JsonSerializer.cpp

)

set(UI_SOURCES
    ${CLIENT_SOURCES_DIR}/MainWindow.cpp

    # Models

    # ViewModels
    ${CLIENT_SOURCES_DIR}/ViewModels/PurchaseOrdersViewModel.cpp
    
    # Dialogs
    ${CLIENT_SOURCES_DIR}/Dialogs/DialogManager.cpp
    ${CLIENT_SOURCES_DIR}/Dialogs/BaseDialog.cpp
    ${CLIENT_SOURCES_DIR}/Dialogs/LoginDialog.cpp
    ${CLIENT_SOURCES_DIR}/Dialogs/RegistrationDialog.cpp

    # Views
    ${CLIENT_SOURCES_DIR}/Views/BaseView.cpp
    ${CLIENT_SOURCES_DIR}/Views/PurchaseOrdersView.cpp
    
    ${CLIENT_SOURCES_DIR}/Views/ViewStyles.hpp
    
)

set(MAIN_SOURCES
    ${CLIENT_SOURCES_DIR}/main.cpp
    ${CLIENT_SOURCES_DIR}/UiManager.cpp
    ${CLIENT_SOURCES_DIR}/SageStoreClient.cpp
    ${CLIENT_SOURCES_DIR}/Utils.cpp
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
