project(${CLIENT_NAME})

set(CLIENT_SOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(CLIENT_BUILD_DIR "${CMAKE_BINARY_DIR}/_client")
set(CLIENT_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Locate Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network)


# Locate conan packages
find_package(spdlog REQUIRED)

set(NETWORK_SOURCES
    ${CLIENT_SOURCES_DIR}/Network/ApiManager.cpp
    ${CLIENT_SOURCES_DIR}/Network/NetworkService.cpp
    ${CLIENT_SOURCES_DIR}/Network/JsonSerializer.cpp
)

set(UI_SOURCES
    ${CLIENT_SOURCES_DIR}/Ui/UiManager.cpp
    ${CLIENT_SOURCES_DIR}/Ui/MainWindow.cpp

    # Models

    # ViewModels
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/PurchaseOrdersViewModel.cpp
    
    # Dialogs
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/DialogManager.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/BaseDialog.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/LoginDialog.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/RegistrationDialog.cpp

    # Views
    ${CLIENT_SOURCES_DIR}/Ui/Views/BaseView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/PurchaseOrdersView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/ViewStyles.hpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/Actions.hpp
)

set(MAIN_SOURCES
    ${CLIENT_SOURCES_DIR}/SageStoreClient.cpp
    ${CLIENT_SOURCES_DIR}/Utils.cpp
)

# All sources
set(ALL_CLIENT_SOURCES
    ${NETWORK_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
    ${CLIENT_SOURCES_DIR}/main.cpp
)

# Adding main code and tests
if (BUILD_CLIENT)
    add_subdirectory(src)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
