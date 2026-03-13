#include "InventoryModule.hpp"

#include <algorithm>
#include <cctype>
#include <limits>

#include "Database/RepositoryManager.hpp"
#include "ServerException.hpp"

#include "common/Entities/Inventory.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/SpdlogConfig.hpp"

#define _M "InventoryModule"

namespace {
std::string getOptionalDatasetValue(const Dataset &request, const std::string &key)
{
    const auto dataIt = request.find(key);
    if (dataIt == request.end() || dataIt->second.empty()) {
        return {};
    }

    return dataIt->second.front();
}

std::string getRequiredDatasetValue(const Dataset &request, const std::string &key,
                                    const char *context)
{
    const auto value = getOptionalDatasetValue(request, key);
    if (value.empty()) {
        SPDLOG_ERROR("{} | Missing required dataset key: {}", context, key);
        throw ServerException(_M, "Missing required field: " + key);
    }

    return value;
}

std::string normalizeBooleanFlag(std::string value)
{
    if (value == "1" || value == "true" || value == "True") {
        return "1";
    }

    return "0";
}

std::string getRequiredNonNegativeInteger(const Dataset &request, const std::string &key,
                                          const char *context)
{
    const auto value = getRequiredDatasetValue(request, key, context);
    if (!std::all_of(value.begin(), value.end(), [](unsigned char c) { return std::isdigit(c); })) {
        SPDLOG_ERROR("{} | Invalid integer for dataset key: {}", context, key);
        throw ServerException(_M, "Invalid integer field: " + key);
    }

    try {
        const auto parsed = std::stoull(value);
        if (parsed > static_cast<unsigned long long>(std::numeric_limits<long long>::max())) {
            SPDLOG_ERROR("{} | Integer is out of range for dataset key: {}", context, key);
            throw ServerException(_M, "Integer field out of range: " + key);
        }
    } catch (const ServerException &) {
        throw;
    } catch (...) {
        SPDLOG_ERROR("{} | Integer is out of range for dataset key: {}", context, key);
        throw ServerException(_M, "Integer field out of range: " + key);
    }

    return value;
}

std::string getRequiredPositiveInteger(const Dataset &request, const std::string &key,
                                       const char *context)
{
    const auto value = getRequiredNonNegativeInteger(request, key, context);
    if (value == "0") {
        SPDLOG_ERROR("{} | Integer must be positive for dataset key: {}", context, key);
        throw ServerException(_M, "Invalid positive integer field: " + key);
    }

    return value;
}

Common::Entities::ProductType buildProductType(const Dataset &request, const std::string &resourceId)
{
    const auto code = getRequiredDatasetValue(request, Common::Entities::ProductType::CODE_KEY,
                                              "InventoryModule::buildProductType");
    const auto name = getRequiredDatasetValue(request, Common::Entities::ProductType::NAME_KEY,
                                              "InventoryModule::buildProductType");
    const auto unit = getRequiredDatasetValue(request, Common::Entities::ProductType::UNIT_KEY,
                                              "InventoryModule::buildProductType");

    auto barcode = getOptionalDatasetValue(request, Common::Entities::ProductType::BARCODE_KEY);
    auto description =
        getOptionalDatasetValue(request, Common::Entities::ProductType::DESCRIPTION_KEY);
    auto lastPrice =
        getOptionalDatasetValue(request, Common::Entities::ProductType::LAST_PRICE_KEY);
    auto isImported =
        getOptionalDatasetValue(request, Common::Entities::ProductType::IS_IMPORTED_KEY);

    if (lastPrice.empty()) {
        lastPrice = "0.0";
    }

    return Common::Entities::ProductType{.id          = resourceId,
                                         .code        = code,
                                         .barcode     = barcode,
                                         .name        = name,
                                         .description = description,
                                         .lastPrice   = lastPrice,
                                         .unit        = unit,
                                         .isImported  = normalizeBooleanFlag(isImported)};
}

Common::Entities::Inventory buildStock(const Dataset &request, const std::string &resourceId)
{
    return Common::Entities::Inventory{
        .id = resourceId,
        .productTypeId = getRequiredDatasetValue(request,
                                                 Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY,
                                                 "InventoryModule::buildStock"),
        .quantityAvailable =
            getRequiredNonNegativeInteger(request,
                                          Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY,
                                          "InventoryModule::buildStock"),
        .employeeId = getRequiredPositiveInteger(request,
                                                 Common::Entities::Inventory::EMPLOYEE_ID_KEY,
                                                 "InventoryModule::buildStock")};
}

Common::Entities::SuppliersProductInfo buildSupplierProduct(const Dataset &request,
                                                            const std::string &resourceId)
{
    return Common::Entities::SuppliersProductInfo{
        .id = resourceId,
        .supplierID = getRequiredPositiveInteger(
            request, Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY,
            "InventoryModule::buildSupplierProduct"),
        .productTypeId = getRequiredPositiveInteger(
            request, Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY,
            "InventoryModule::buildSupplierProduct"),
        .code = getRequiredDatasetValue(request, Common::Entities::SuppliersProductInfo::CODE_KEY,
                                        "InventoryModule::buildSupplierProduct")};
}
} // namespace

InventoryModule::InventoryModule(RepositoryManager &repositoryManager)
{
    SPDLOG_TRACE("InventoryModule::InventoryModule");

    m_inventoryRepository    = std::move(repositoryManager.getInventoryRepository());
    m_productTypesRepository = std::move(repositoryManager.getProductTypeRepository());
    m_suppliersRepository    = std::move(repositoryManager.getSupplierRepository());
    m_suppliersProductInfoRepository =
        std::move(repositoryManager.getSuppliersProductInfoRepository());
}

InventoryModule::~InventoryModule() { SPDLOG_TRACE("InventoryModule::~InventoryModule"); }

ResponseData InventoryModule::executeTask(const RequestData &requestData)
{
    SPDLOG_TRACE("InventoryModule::executeTask");

    if (requestData.submodule == "product-types") {
        if (requestData.method == "GET") {
            return getProductTypes();
        }

        if (requestData.method == "POST") {
            addProductType(requestData.dataset);
            return {};
        }

        if (requestData.method == "PUT") {
            editProductType(requestData.dataset, requestData.resourceId);
            return {};
        }

        if (requestData.method == "DELETE") {
            deleteProductType(requestData.resourceId);
            return {};
        }

        throw ServerException(_M, "Unrecognized method");
    }

    if (requestData.submodule == "stocks") {
        if (requestData.method == "GET") {
            return getStocks();
        }

        if (requestData.method == "POST") {
            addStock(requestData.dataset);
            return {};
        }

        if (requestData.method == "PUT") {
            editStock(requestData.dataset, requestData.resourceId);
            return {};
        }

        if (requestData.method == "DELETE") {
            deleteStock(requestData.resourceId);
            return {};
        }

        throw ServerException(_M, "Unrecognized method");
    }

    if (requestData.submodule == "supplier-products") {
        if (requestData.method == "GET") {
            return getSupplierProducts();
        }

        if (requestData.method == "POST") {
            addSupplierProduct(requestData.dataset);
            return {};
        }

        if (requestData.method == "PUT") {
            editSupplierProduct(requestData.dataset, requestData.resourceId);
            return {};
        }

        if (requestData.method == "DELETE") {
            deleteSupplierProduct(requestData.resourceId);
            return {};
        }

        throw ServerException(_M, "Unrecognized method");
    }

    throw ServerException(_M, "Unrecognized inventory task");
}

ResponseData InventoryModule::getProductTypes() const
{
    SPDLOG_TRACE("InventoryModule::getProductTypes");

    ResponseData response;
    response.dataset[Common::Entities::ProductType::ID_KEY]          = {};
    response.dataset[Common::Entities::ProductType::CODE_KEY]        = {};
    response.dataset[Common::Entities::ProductType::BARCODE_KEY]     = {};
    response.dataset[Common::Entities::ProductType::NAME_KEY]        = {};
    response.dataset[Common::Entities::ProductType::DESCRIPTION_KEY] = {};
    response.dataset[Common::Entities::ProductType::LAST_PRICE_KEY]  = {};
    response.dataset[Common::Entities::ProductType::UNIT_KEY]        = {};
    response.dataset[Common::Entities::ProductType::IS_IMPORTED_KEY] = {};

    const auto productTypes = m_productTypesRepository->getAll();
    for (const auto &productType : productTypes) {
        response.dataset[Common::Entities::ProductType::ID_KEY].emplace_back(productType.id);
        response.dataset[Common::Entities::ProductType::CODE_KEY].emplace_back(productType.code);
        response.dataset[Common::Entities::ProductType::BARCODE_KEY].emplace_back(productType.barcode);
        response.dataset[Common::Entities::ProductType::NAME_KEY].emplace_back(productType.name);
        response.dataset[Common::Entities::ProductType::DESCRIPTION_KEY].emplace_back(
            productType.description);
        response.dataset[Common::Entities::ProductType::LAST_PRICE_KEY].emplace_back(
            productType.lastPrice);
        response.dataset[Common::Entities::ProductType::UNIT_KEY].emplace_back(productType.unit);
        response.dataset[Common::Entities::ProductType::IS_IMPORTED_KEY].emplace_back(
            productType.isImported);
    }

    return response;
}

void InventoryModule::addProductType(const Dataset &request) const
{
    SPDLOG_TRACE("InventoryModule::addProductType");

    const auto productType = buildProductType(request, "");
    const auto existingProductTypes = m_productTypesRepository->getByField(
        Common::Entities::ProductType::CODE_KEY, productType.code);
    if (!existingProductTypes.empty()) {
        throw ServerException(_M, "Product type with this code already exists");
    }

    m_productTypesRepository->add(productType);
}

void InventoryModule::editProductType(const Dataset &request, const std::string &resourceId) const
{
    SPDLOG_TRACE("InventoryModule::editProductType");

    if (resourceId.empty()) {
        throw ServerException(_M, "Product type ID is empty");
    }

    if (m_productTypesRepository
            ->getByField(Common::Entities::ProductType::ID_KEY, resourceId)
            .empty()) {
        throw ServerException(_M, "Product type does not exist");
    }

    const auto productType = buildProductType(request, resourceId);
    const auto productTypesByCode = m_productTypesRepository->getByField(
        Common::Entities::ProductType::CODE_KEY, productType.code);

    for (const auto &existingProductType : productTypesByCode) {
        if (existingProductType.id != resourceId) {
            throw ServerException(_M, "Product type with this code already exists");
        }
    }

    m_productTypesRepository->update(productType);
}

void InventoryModule::deleteProductType(const std::string &resourceId) const
{
    SPDLOG_TRACE("InventoryModule::deleteProductType");

    if (resourceId.empty()) {
        throw ServerException(_M, "Product type ID is empty");
    }

    if (m_productTypesRepository
            ->getByField(Common::Entities::ProductType::ID_KEY, resourceId)
            .empty()) {
        throw ServerException(_M, "Product type does not exist");
    }

    m_productTypesRepository->deleteResource(resourceId);
}

ResponseData InventoryModule::getStocks() const
{
    SPDLOG_TRACE("InventoryModule::getStocks");

    ResponseData response;
    response.dataset[Common::Entities::Inventory::ID_KEY]                 = {};
    response.dataset[Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY]    = {};
    response.dataset[Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY] = {};
    response.dataset[Common::Entities::Inventory::EMPLOYEE_ID_KEY]        = {};

    const auto inventoryRecords = m_inventoryRepository->getAll();
    for (const auto &stock : inventoryRecords) {
        response.dataset[Common::Entities::Inventory::ID_KEY].emplace_back(stock.id);
        response.dataset[Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY].emplace_back(
            stock.productTypeId);
        response.dataset[Common::Entities::Inventory::QUANTITY_AVAILABLE_KEY].emplace_back(
            stock.quantityAvailable);
        response.dataset[Common::Entities::Inventory::EMPLOYEE_ID_KEY].emplace_back(
            stock.employeeId);
    }

    return response;
}

ResponseData InventoryModule::getSupplierProducts() const
{
    SPDLOG_TRACE("InventoryModule::getSupplierProducts");

    ResponseData response;
    response.dataset[Common::Entities::SuppliersProductInfo::ID_KEY]              = {};
    response.dataset[Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY]     = {};
    response.dataset[Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY] = {};
    response.dataset[Common::Entities::SuppliersProductInfo::CODE_KEY]            = {};

    const auto mappings = m_suppliersProductInfoRepository->getAll();
    for (const auto &mapping : mappings) {
        response.dataset[Common::Entities::SuppliersProductInfo::ID_KEY].emplace_back(mapping.id);
        response.dataset[Common::Entities::SuppliersProductInfo::SUPPLIER_ID_KEY].emplace_back(
            mapping.supplierID);
        response.dataset[Common::Entities::SuppliersProductInfo::PRODUCT_TYPE_ID_KEY].emplace_back(
            mapping.productTypeId);
        response.dataset[Common::Entities::SuppliersProductInfo::CODE_KEY].emplace_back(
            mapping.code);
    }

    return response;
}

void InventoryModule::addStock(const Dataset &request) const
{
    SPDLOG_TRACE("InventoryModule::addStock");

    const auto stock = buildStock(request, "");
    if (m_productTypesRepository
            ->getByField(Common::Entities::ProductType::ID_KEY, stock.productTypeId)
            .empty()) {
        throw ServerException(_M, "Referenced product type does not exist");
    }

    if (!m_inventoryRepository
             ->getByField(Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, stock.productTypeId)
             .empty()) {
        throw ServerException(_M, "Stock record for this product type already exists");
    }

    m_inventoryRepository->add(stock);
}

void InventoryModule::editStock(const Dataset &request, const std::string &resourceId) const
{
    SPDLOG_TRACE("InventoryModule::editStock");

    if (resourceId.empty()) {
        throw ServerException(_M, "Stock ID is empty");
    }

    if (m_inventoryRepository
            ->getByField(Common::Entities::Inventory::ID_KEY, resourceId)
            .empty()) {
        throw ServerException(_M, "Stock record does not exist");
    }

    const auto stock = buildStock(request, resourceId);
    if (m_productTypesRepository
            ->getByField(Common::Entities::ProductType::ID_KEY, stock.productTypeId)
            .empty()) {
        throw ServerException(_M, "Referenced product type does not exist");
    }

    const auto stocksByProductType = m_inventoryRepository->getByField(
        Common::Entities::Inventory::PRODUCT_TYPE_ID_KEY, stock.productTypeId);
    for (const auto &existingStock : stocksByProductType) {
        if (existingStock.id != resourceId) {
            throw ServerException(_M, "Stock record for this product type already exists");
        }
    }

    m_inventoryRepository->update(stock);
}

void InventoryModule::deleteStock(const std::string &resourceId) const
{
    SPDLOG_TRACE("InventoryModule::deleteStock");

    if (resourceId.empty()) {
        throw ServerException(_M, "Stock ID is empty");
    }

    if (m_inventoryRepository
            ->getByField(Common::Entities::Inventory::ID_KEY, resourceId)
            .empty()) {
        throw ServerException(_M, "Stock record does not exist");
    }

    m_inventoryRepository->deleteResource(resourceId);
}

void InventoryModule::addSupplierProduct(const Dataset &request) const
{
    SPDLOG_TRACE("InventoryModule::addSupplierProduct");

    const auto mapping = buildSupplierProduct(request, "");
    if (m_suppliersRepository
            ->getByField(Common::Entities::Supplier::ID_KEY, mapping.supplierID)
            .empty()) {
        throw ServerException(_M, "Referenced supplier does not exist");
    }

    if (m_productTypesRepository
            ->getByField(Common::Entities::ProductType::ID_KEY, mapping.productTypeId)
            .empty()) {
        throw ServerException(_M, "Referenced product type does not exist");
    }

    m_suppliersProductInfoRepository->add(mapping);
}

void InventoryModule::editSupplierProduct(const Dataset &request,
                                          const std::string &resourceId) const
{
    SPDLOG_TRACE("InventoryModule::editSupplierProduct");

    if (resourceId.empty()) {
        throw ServerException(_M, "Supplier product mapping ID is empty");
    }

    if (m_suppliersProductInfoRepository
            ->getByField(Common::Entities::SuppliersProductInfo::ID_KEY, resourceId)
            .empty()) {
        throw ServerException(_M, "Supplier product mapping does not exist");
    }

    const auto mapping = buildSupplierProduct(request, resourceId);
    if (m_suppliersRepository
            ->getByField(Common::Entities::Supplier::ID_KEY, mapping.supplierID)
            .empty()) {
        throw ServerException(_M, "Referenced supplier does not exist");
    }

    if (m_productTypesRepository
            ->getByField(Common::Entities::ProductType::ID_KEY, mapping.productTypeId)
            .empty()) {
        throw ServerException(_M, "Referenced product type does not exist");
    }

    m_suppliersProductInfoRepository->update(mapping);
}

void InventoryModule::deleteSupplierProduct(const std::string &resourceId) const
{
    SPDLOG_TRACE("InventoryModule::deleteSupplierProduct");

    if (resourceId.empty()) {
        throw ServerException(_M, "Supplier product mapping ID is empty");
    }

    if (m_suppliersProductInfoRepository
            ->getByField(Common::Entities::SuppliersProductInfo::ID_KEY, resourceId)
            .empty()) {
        throw ServerException(_M, "Supplier product mapping does not exist");
    }

    m_suppliersProductInfoRepository->deleteResource(resourceId);
}
