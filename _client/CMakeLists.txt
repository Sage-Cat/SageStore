project(${CLIENT_NAME})

set(CLIENT_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CLIENT_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(CLIENT_BUILD_DIR ${CMAKE_BINARY_DIR}/_client)

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
)

set(UI_SOURCES
    ${CLIENT_SOURCES_DIR}/Ui/MainWindow.cpp

    # Models
    ${CLIENT_SOURCES_DIR}/Ui/Models/BaseModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/UsersManagementModel.cpp

    # ViewModels
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/PurchaseOrdersViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/MainMenuActions.hpp
    
    # Dialogs
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/DialogManager.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/BaseDialog.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/LoginDialog.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/RegistrationDialog.cpp

    # Views
    ${CLIENT_SOURCES_DIR}/Ui/Views/BaseView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/PurchaseOrdersView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/ViewStyles.hpp
)

set(MAIN_SOURCES
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
