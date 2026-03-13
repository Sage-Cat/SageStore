#pragma once

#include <algorithm>
#include <vector>

#include "Network/ApiManager.hpp"

#include "mocks/NetworkServiceMock.hpp"

class ApiManagerMock : public ApiManager {
public:
    ApiManagerMock() : ApiManager(*(new NetworkServiceMock))
    {
        m_roles = {Common::Entities::Role{.id = "1", .name = "Administrator"},
                   Common::Entities::Role{.id = "2", .name = "Manager"}};
        m_productTypes = {
            Common::Entities::ProductType{.id = "1",
                                          .code = "PT-001",
                                          .barcode = "123",
                                          .name = "Oil",
                                          .description = "Synthetic oil",
                                          .lastPrice = "10.50",
                                          .unit = "pcs",
                                          .isImported = "0"},
            Common::Entities::ProductType{.id = "2",
                                          .code = "PT-002",
                                          .barcode = "456",
                                          .name = "Brake fluid",
                                          .description = "DOT-4",
                                          .lastPrice = "12.00",
                                          .unit = "bottle",
                                          .isImported = "1"}};
        m_stocks = {Common::Entities::Inventory{.id = "1",
                                                .productTypeId = "1",
                                                .quantityAvailable = "10",
                                                .employeeId = "1"}};
    }

    int usersRequestCount() const { return m_getUsersCalls; }
    int rolesRequestCount() const { return m_getRolesCalls; }
    int productTypesRequestCount() const { return m_getProductTypesCalls; }
    int stocksRequestCount() const { return m_getStocksCalls; }
    void setProductTypes(const std::vector<Common::Entities::ProductType> &productTypes)
    {
        m_productTypes = productTypes;
    }

public slots:
    void addUser(const Common::Entities::User &user) override { emit userAdded(); }
    void editUser(const Common::Entities::User &user) override { emit userEdited(); }
    void deleteUser(const QString &id) override { emit userDeleted(); }
    void getUsers() override
    {
        ++m_getUsersCalls;
        emit usersList({});
    }
    void editRole(const Common::Entities::Role &role) override
    {
        auto it = std::find_if(m_roles.begin(), m_roles.end(), [&role](const auto &existingRole) {
            return existingRole.id == role.id;
        });
        if (it != m_roles.end()) {
            *it = role;
        }
        emit roleEdited();
    }
    void deleteRole(const QString &id) override
    {
        std::erase_if(m_roles, [&id](const auto &role) { return role.id == id.toStdString(); });
        emit roleDeleted();
    }
    void getRoleList() override
    {
        ++m_getRolesCalls;
        emit rolesList(m_roles);
    }
    void createRole(const Common::Entities::Role &role) override
    {
        auto createdRole = role;
        createdRole.id   = std::to_string(m_nextRoleId++);
        m_roles.push_back(createdRole);
        emit roleCreated();
    }
    void getProductTypes() override
    {
        ++m_getProductTypesCalls;
        emit productTypesList(m_productTypes);
    }
    void createProductType(const Common::Entities::ProductType &productType) override
    {
        auto createdProductType = productType;
        createdProductType.id   = std::to_string(m_nextProductTypeId++);
        m_productTypes.push_back(createdProductType);
        emit productTypeCreated();
    }
    void editProductType(const Common::Entities::ProductType &productType) override
    {
        auto it = std::find_if(m_productTypes.begin(), m_productTypes.end(),
                               [&productType](const auto &existingProductType) {
                                   return existingProductType.id == productType.id;
                               });
        if (it != m_productTypes.end()) {
            *it = productType;
        }
        emit productTypeEdited();
    }
    void deleteProductType(const QString &id) override
    {
        std::erase_if(m_productTypes,
                      [&id](const auto &productType) { return productType.id == id.toStdString(); });
        emit productTypeDeleted();
    }
    void getStocks() override
    {
        ++m_getStocksCalls;
        emit stocksList(m_stocks);
    }
    void createStock(const Common::Entities::Inventory &stock) override
    {
        auto createdStock = stock;
        createdStock.id   = std::to_string(m_nextStockId++);
        m_stocks.push_back(createdStock);
        emit stockCreated();
    }
    void editStock(const Common::Entities::Inventory &stock) override
    {
        auto it = std::find_if(m_stocks.begin(), m_stocks.end(),
                               [&stock](const auto &existingStock) {
                                   return existingStock.id == stock.id;
                               });
        if (it != m_stocks.end()) {
            *it = stock;
        }
        emit stockEdited();
    }
    void deleteStock(const QString &id) override
    {
        std::erase_if(m_stocks,
                      [&id](const auto &stock) { return stock.id == id.toStdString(); });
        emit stockDeleted();
    }
    void emitError(const QString &errorMessage) { emit errorOccurred(errorMessage); }

private:
    int m_getUsersCalls{0};
    int m_getRolesCalls{0};
    int m_getProductTypesCalls{0};
    int m_getStocksCalls{0};
    int m_nextRoleId{3};
    int m_nextProductTypeId{3};
    int m_nextStockId{2};
    std::vector<Common::Entities::Role> m_roles;
    std::vector<Common::Entities::ProductType> m_productTypes;
    std::vector<Common::Entities::Inventory> m_stocks;
};
