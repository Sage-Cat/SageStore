project(${CLIENT_NAME})

set(CLIENT_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CLIENT_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
set(CLIENT_BUILD_DIR ${CMAKE_BINARY_DIR}/_client)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Locate Qt packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui Network Test)

# Locate conan packages
find_package(spdlog REQUIRED)

set(NETWORK_SOURCES
    ${CLIENT_SOURCES_DIR}/Network/ApiManager.cpp
    ${CLIENT_SOURCES_DIR}/Network/NetworkService.cpp
)

set(UI_SOURCES
    ${CLIENT_SOURCES_DIR}/Ui/MainWindow.cpp
    ${CLIENT_SOURCES_DIR}/Ui/MainMenuActions.hpp
    ${CLIENT_SOURCES_DIR}/Ui/StartupController.cpp
    ${CLIENT_SOURCES_DIR}/Ui/UiScale.hpp

    # Models
    ${CLIENT_SOURCES_DIR}/Ui/Models/AnalyticsModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/BaseModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/LogsModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/ManagementModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/ProductTypesModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/PurchaseModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/SalesModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/SupplierCatalogModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/StocksModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Models/UsersManagementModel.cpp

    # ViewModels
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/AnalyticsViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/BaseViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/LogsViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/ManagementViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/ProductTypesViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/PurchaseViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/SalesViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/SupplierCatalogViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/StocksViewModel.cpp
    ${CLIENT_SOURCES_DIR}/Ui/ViewModels/UsersManagementViewModel.cpp
    
    # Dialogs
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/DialogManager.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/BaseDialog.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/LoginDialog.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Dialogs/RegistrationDialog.cpp

    # Views
    ${CLIENT_SOURCES_DIR}/Ui/Views/AnalyticsView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/BaseView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/LogsView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/ManagementView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/ModuleStatusView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/ProductTypesView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/PurchaseView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/RolesView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/SalesView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/SettingsView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/StocksView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/SupplierCatalogView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/UsersView.cpp
    ${CLIENT_SOURCES_DIR}/Ui/Views/ViewStyles.hpp
)

set(MAIN_SOURCES
    ${CLIENT_SOURCES_DIR}/Localization/TsTranslator.cpp
    ${CLIENT_SOURCES_DIR}/Settings/AppSettings.cpp
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

if(BUILD_CLIENT OR BUILD_TESTS)
    add_subdirectory(tools/debug_cli)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
