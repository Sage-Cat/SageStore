#include "ApiManager.hpp"

#include "Network/NetworkService.hpp"
#include "common/Endpoints.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Keys.hpp"
#include "common/SpdlogConfig.hpp"

ApiManager::ApiManager(NetworkService &networkService) : m_networkService(networkService)
{
    SPDLOG_TRACE("ApiManager::ApiManager");

    connect(&m_networkService, &NetworkService::connected, this, &ApiManager::ready);
    connect(&m_networkService, &NetworkService::responseReceived, this,
            &ApiManager::handleResponse);

    setupHandlers();
}

ApiManager::~ApiManager() { SPDLOG_TRACE("ApiManager::~ApiManager"); }

void ApiManager::loginUser(const QString &username, const QString &password)
{
    SPDLOG_TRACE("ApiManager::loginUser username={}", username.toStdString());

    Dataset dataset;
    dataset[Common::Entities::User::USERNAME_KEY] = {username.toStdString()};
    dataset[Common::Entities::User::PASSWORD_KEY] = {password.toStdString()};
    m_networkService.sendRequest(Endpoints::Users::LOGIN, Method::POST, dataset);
}

void ApiManager::getUsers()
{
    SPDLOG_TRACE("ApiManager::getUsers");

    m_networkService.sendRequest(Endpoints::Users::USERS, Method::GET);
}

void ApiManager::addUser(const Common::Entities::User &user)
{
    SPDLOG_TRACE("ApiManager::addUser");

    Dataset dataset;
    dataset[Common::Entities::User::ID_KEY]       = {user.id};
    dataset[Common::Entities::User::USERNAME_KEY] = {user.username};
    dataset[Common::Entities::User::PASSWORD_KEY] = {user.password};
    dataset[Common::Entities::User::ROLE_ID_KEY]  = {user.roleId};

    m_networkService.sendRequest(Endpoints::Users::USERS, Method::POST, dataset);
}

void ApiManager::editUser(const Common::Entities::User &user)
{
    SPDLOG_TRACE("ApiManager::editUser");

    Dataset dataset;
    dataset[Common::Entities::User::ID_KEY]       = {user.id};
    dataset[Common::Entities::User::USERNAME_KEY] = {user.username};
    dataset[Common::Entities::User::PASSWORD_KEY] = {user.password};
    dataset[Common::Entities::User::ROLE_ID_KEY]  = {user.roleId};

    m_networkService.sendRequest(Endpoints::Users::USERS, Method::PUT, dataset, user.id);
}

void ApiManager::deleteUser(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteUser");

    m_networkService.sendRequest(Endpoints::Users::USERS, Method::DEL, {}, id.toStdString());
}

void ApiManager::getRoleList()
{
    SPDLOG_TRACE("ApiManager::getRole");

    m_networkService.sendRequest(Endpoints::Users::ROLES, Method::GET);
}

void ApiManager::createRole(const Common::Entities::Role &role)
{
    SPDLOG_TRACE("ApiManager::postRole");
    Dataset dataset;
    dataset[Common::Entities::Role::NAME_KEY] = {role.name};
    dataset[Common::Entities::Role::ID_KEY]   = {};

    m_networkService.sendRequest(Endpoints::Users::ROLES, Method::POST, dataset);
}

void ApiManager::editRole(const Common::Entities::Role &role)
{
    SPDLOG_TRACE("ApiManager::editRole");
    Dataset dataset;
    dataset[Common::Entities::Role::NAME_KEY] = {role.name};

    m_networkService.sendRequest(Endpoints::Users::ROLES, Method::PUT, dataset, role.id);
}

void ApiManager::deleteRole(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteRole");

    m_networkService.sendRequest(Endpoints::Users::ROLES, Method::DEL, {}, id.toStdString());
}

void ApiManager::getProductTypes()
{
    SPDLOG_TRACE("ApiManager::getProductTypes");

    m_networkService.sendRequest(Endpoints::Inventory::PRODUCT_TYPES, Method::GET);
}

void ApiManager::createProductType(const Common::Entities::ProductType &productType)
{
    SPDLOG_TRACE("ApiManager::createProductType");

    Dataset dataset;
    dataset[Common::Entities::ProductType::CODE_KEY]        = {productType.code};
    dataset[Common::Entities::ProductType::BARCODE_KEY]     = {productType.barcode};
    dataset[Common::Entities::ProductType::NAME_KEY]        = {productType.name};
    dataset[Common::Entities::ProductType::DESCRIPTION_KEY] = {productType.description};
    dataset[Common::Entities::ProductType::LAST_PRICE_KEY]  = {productType.lastPrice};
    dataset[Common::Entities::ProductType::UNIT_KEY]        = {productType.unit};
    dataset[Common::Entities::ProductType::IS_IMPORTED_KEY] = {productType.isImported};

    m_networkService.sendRequest(Endpoints::Inventory::PRODUCT_TYPES, Method::POST, dataset);
}

void ApiManager::editProductType(const Common::Entities::ProductType &productType)
{
    SPDLOG_TRACE("ApiManager::editProductType");

    Dataset dataset;
    dataset[Common::Entities::ProductType::CODE_KEY]        = {productType.code};
    dataset[Common::Entities::ProductType::BARCODE_KEY]     = {productType.barcode};
    dataset[Common::Entities::ProductType::NAME_KEY]        = {productType.name};
    dataset[Common::Entities::ProductType::DESCRIPTION_KEY] = {productType.description};
    dataset[Common::Entities::ProductType::LAST_PRICE_KEY]  = {productType.lastPrice};
    dataset[Common::Entities::ProductType::UNIT_KEY]        = {productType.unit};
    dataset[Common::Entities::ProductType::IS_IMPORTED_KEY] = {productType.isImported};

    m_networkService.sendRequest(Endpoints::Inventory::PRODUCT_TYPES, Method::PUT, dataset,
                                 productType.id);
}

void ApiManager::deleteProductType(const QString &id)
{
    SPDLOG_TRACE("ApiManager::deleteProductType");

    m_networkService.sendRequest(Endpoints::Inventory::PRODUCT_TYPES, Method::DEL, {},
                                 id.toStdString());
}

void ApiManager::setupHandlers()
{
    SPDLOG_TRACE("ApiManager::setupHandlers");
    m_responseHandlers[Endpoints::Users::LOGIN] = [this](Method method, const Dataset &dataset) {
        handleLoginResponse(method, dataset);
    };
    m_responseHandlers[Endpoints::Users::USERS] = [this](Method method, const Dataset &dataset) {
        handleUsers(method, dataset);
    };
    m_responseHandlers[Endpoints::Users::ROLES] = [this](Method method, const Dataset &dataset) {
        handleRoles(method, dataset);
    };
    m_responseHandlers[Endpoints::Inventory::PRODUCT_TYPES] =
        [this](Method method, const Dataset &dataset) { handleProductTypes(method, dataset); };
}

void ApiManager::handleResponse(const std::string &endpoint, Method method, const Dataset &dataset)
{
    SPDLOG_DEBUG("ApiManager::handleResponse for endpoint={}", endpoint);
    auto handler = m_responseHandlers.find(endpoint);
    if (handler != m_responseHandlers.end()) {
        std::string errorMsg{};
        if (dataset.contains(Keys::_ERROR)) {
            for (const auto &errorData : dataset.at(Keys::_ERROR)) {
                errorMsg += errorData + "\n";
            }
        }

        if (errorMsg.empty()) {
            handler->second(method, dataset);
        } else {
            handleError(errorMsg);
        }
    } else {
        SPDLOG_ERROR("ApiManager::handleResponse | Can't find handler for endpoint={}", endpoint);
    }
}

void ApiManager::handleError(const std::string &errorMessage)
{
    SPDLOG_TRACE("ApiManager::handleError | {}", errorMessage);
    emit errorOccurred(QString::fromStdString(errorMessage));
}

void ApiManager::handleRoles(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleRolesList(dataset);
        break;
    case Method::POST:
        emit roleCreated();
        break;
    case Method::PUT:
        emit roleEdited();
        break;
    case Method::DEL:
        emit roleDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleRoles | Got unhandled method");
        break;
    }
}

void ApiManager::handleProductTypes(Method method, const Dataset &dataset)
{
    switch (method) {
    case Method::GET:
        handleProductTypesList(dataset);
        break;
    case Method::POST:
        emit productTypeCreated();
        break;
    case Method::PUT:
        emit productTypeEdited();
        break;
    case Method::DEL:
        emit productTypeDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleProductTypes | Got unhandled method");
        break;
    }
}

void ApiManager::handleLoginResponse(Method, const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleLoginResponse");

    try {
        const auto &id_list      = dataset.at(Common::Entities::User::ID_KEY);
        const auto &role_id_list = dataset.at(Common::Entities::User::ROLE_ID_KEY);

        if (id_list.empty() || role_id_list.empty()) {
            handleError("ApiManager::handleLoginResponse | empty lists");
            return;
        }

        const std::string &id     = id_list.front();
        const std::string &roleId = role_id_list.front();

        if (id.empty() || roleId.empty()) {
            handleError("ApiManager::handleLoginResponse | empty id or roleId");
        } else {
            emit loginSuccess(QString::fromStdString(id), QString::fromStdString(roleId));
        }
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("Missing required keys in dataset: {}", e.what());
        handleError("ApiManager::handleLoginResponse | missing key in dataset");
    }
}

void ApiManager::handleUsers(Method method, const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleUsers");

    switch (method) {
    case Method::GET:
        handleUsersList(dataset);
        break;
    case Method::POST:
        emit userAdded();
        break;
    case Method::PUT:
        emit userEdited();
        break;
    case Method::DEL:
        emit userDeleted();
        break;
    default:
        SPDLOG_WARN("ApiManager::handleRoles | Got unhandled method");
        break;
    }
}

void ApiManager::handleRolesList(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleRolesList");

    try {
        if (!dataset.contains(Common::Entities::Role::ID_KEY) ||
            !dataset.contains(Common::Entities::Role::NAME_KEY)) {
            handleError("dataset doesn't contain ID or NAME lists");
            return;
        }

        const auto &idList   = dataset.at(Common::Entities::Role::ID_KEY);
        const auto &nameList = dataset.at(Common::Entities::Role::NAME_KEY);

        if (idList.size() != nameList.size()) {
            handleError("ID and Name lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::Role> roles;
        auto idIt   = idList.begin();
        auto nameIt = nameList.begin();

        while (idIt != idList.end() && nameIt != nameList.end()) {
            Common::Entities::Role role{.id = *idIt, .name = *nameIt};
            roles.push_back(role);

            ++idIt;
            ++nameIt;
        }

        emit rolesList(roles);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleRolesList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleUsersList(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleUsersList");

    try {
        if (!dataset.contains(Common::Entities::User::ID_KEY) ||
            !dataset.contains(Common::Entities::User::USERNAME_KEY) ||
            !dataset.contains(Common::Entities::User::ROLE_ID_KEY)) {
            handleError("Dataset doesn't contain required User keys");
            return;
        }

        const auto &idList       = dataset.at(Common::Entities::User::ID_KEY);
        const auto &usernameList = dataset.at(Common::Entities::User::USERNAME_KEY);
        const auto &roleIdList   = dataset.at(Common::Entities::User::ROLE_ID_KEY);

        if (idList.size() != usernameList.size() || usernameList.size() != roleIdList.size()) {
            handleError("User lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::User> users;
        auto idIt       = idList.begin();
        auto usernameIt = usernameList.begin();
        auto roleIdIt   = roleIdList.begin();

        while (idIt != idList.end() && usernameIt != usernameList.end() &&
               roleIdIt != roleIdList.end()) {
            Common::Entities::User user{.id = *idIt, .username = *usernameIt, .roleId = *roleIdIt};
            users.push_back(user);

            ++idIt;
            ++usernameIt;
            ++roleIdIt;
        }

        emit usersList(users);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleUsersList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}

void ApiManager::handleProductTypesList(const Dataset &dataset)
{
    SPDLOG_TRACE("ApiManager::handleProductTypesList");

    try {
        const auto &idList          = dataset.at(Common::Entities::ProductType::ID_KEY);
        const auto &codeList        = dataset.at(Common::Entities::ProductType::CODE_KEY);
        const auto &barcodeList     = dataset.at(Common::Entities::ProductType::BARCODE_KEY);
        const auto &nameList        = dataset.at(Common::Entities::ProductType::NAME_KEY);
        const auto &descriptionList = dataset.at(Common::Entities::ProductType::DESCRIPTION_KEY);
        const auto &lastPriceList   = dataset.at(Common::Entities::ProductType::LAST_PRICE_KEY);
        const auto &unitList        = dataset.at(Common::Entities::ProductType::UNIT_KEY);
        const auto &isImportedList  = dataset.at(Common::Entities::ProductType::IS_IMPORTED_KEY);

        const auto itemCount = idList.size();
        if (codeList.size() != itemCount || barcodeList.size() != itemCount ||
            nameList.size() != itemCount || descriptionList.size() != itemCount ||
            lastPriceList.size() != itemCount || unitList.size() != itemCount ||
            isImportedList.size() != itemCount) {
            handleError("ProductType lists have different sizes.");
            return;
        }

        std::vector<Common::Entities::ProductType> productTypes;
        auto idIt          = idList.begin();
        auto codeIt        = codeList.begin();
        auto barcodeIt     = barcodeList.begin();
        auto nameIt        = nameList.begin();
        auto descriptionIt = descriptionList.begin();
        auto lastPriceIt   = lastPriceList.begin();
        auto unitIt        = unitList.begin();
        auto importedIt    = isImportedList.begin();

        while (idIt != idList.end()) {
            productTypes.push_back(Common::Entities::ProductType{.id = *idIt,
                                                                 .code = *codeIt,
                                                                 .barcode = *barcodeIt,
                                                                 .name = *nameIt,
                                                                 .description = *descriptionIt,
                                                                 .lastPrice = *lastPriceIt,
                                                                 .unit = *unitIt,
                                                                 .isImported = *importedIt});
            ++idIt;
            ++codeIt;
            ++barcodeIt;
            ++nameIt;
            ++descriptionIt;
            ++lastPriceIt;
            ++unitIt;
            ++importedIt;
        }

        emit productTypesList(productTypes);
    } catch (const std::out_of_range &e) {
        SPDLOG_ERROR("handleProductTypesList | Key missing in dataset: {}", e.what());
        handleError("Key missing in dataset");
    }
}
