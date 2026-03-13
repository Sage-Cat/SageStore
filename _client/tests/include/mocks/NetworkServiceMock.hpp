#pragma once

#include "Network/NetworkService.hpp"
#include "common/Endpoints.hpp"
#include "common/Keys.hpp"

class NetworkServiceMock : public NetworkService {
public:
    NetworkServiceMock() : NetworkService(NetworkService::ServerConfig(), nullptr) {}

    void sendRequest(std::string endpoint, Method method = Method::GET,
                     const Dataset &dataset         = Dataset(),
                     const std::string &resource_id = "") override
    {
        Dataset serverResponse{{Keys::_ERROR, {"Error message"}}};

        if (endpoint == Endpoints::Users::LOGIN && method == Method::POST) {
            serverResponse = {{Common::Entities::User::ID_KEY, {"0"}},
                              {Common::Entities::User::ROLE_ID_KEY, {"0"}}};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::GET) {
            serverResponse = {{Common::Entities::User::ID_KEY, {"1", "2"}},
                              {Common::Entities::User::USERNAME_KEY, {"user1", "user2"}},
                              {Common::Entities::User::ROLE_ID_KEY, {"1", "2"}}};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::POST) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::USERS && method == Method::DEL) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::GET) {
            serverResponse = {{Common::Entities::Role::ID_KEY, {"1", "2", "3"}},
                              {Common::Entities::Role::NAME_KEY, {"adm", "asd", "Asdf"}}};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::POST) {
            serverResponse = {{Common::Entities::Role::ID_KEY, {"0"}}};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Users::ROLES && method == Method::DEL) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::PRODUCT_TYPES && method == Method::GET) {
            serverResponse = {{Common::Entities::ProductType::ID_KEY, {"1", "2"}},
                              {Common::Entities::ProductType::CODE_KEY, {"PT-001", "PT-002"}},
                              {Common::Entities::ProductType::BARCODE_KEY, {"123", "456"}},
                              {Common::Entities::ProductType::NAME_KEY, {"Oil", "Brake fluid"}},
                              {Common::Entities::ProductType::DESCRIPTION_KEY,
                               {"Synthetic oil", "DOT-4"}},
                              {Common::Entities::ProductType::LAST_PRICE_KEY, {"10.50", "12.00"}},
                              {Common::Entities::ProductType::UNIT_KEY, {"pcs", "bottle"}},
                              {Common::Entities::ProductType::IS_IMPORTED_KEY, {"0", "1"}}};
        } else if (endpoint == Endpoints::Inventory::PRODUCT_TYPES && method == Method::POST) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::PRODUCT_TYPES && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::PRODUCT_TYPES && method == Method::DEL) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::STOCKS && method == Method::GET) {
            serverResponse = {{Common::Entities::Inventory::ID_KEY, {"1", "2"}},
                              {Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, {"1", "2"}},
                              {Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY, {"10", "4"}},
                              {Common::Entities::Inventory::EMPLOYEE_ID_KEY, {"1", "1"}}};
        } else if (endpoint == Endpoints::Inventory::STOCKS && method == Method::POST) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::STOCKS && method == Method::PUT) {
            serverResponse = {};
        } else if (endpoint == Endpoints::Inventory::STOCKS && method == Method::DEL) {
            serverResponse = {};
        }
        emit responseReceived(endpoint, method, serverResponse);
    }
};
