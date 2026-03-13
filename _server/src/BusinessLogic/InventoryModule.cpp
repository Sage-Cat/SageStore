#include "InventoryModule.hpp"

#include "Database/RepositoryManager.hpp"
#include "ServerException.hpp"

#include "common/Entities/ProductType.hpp"
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
} // namespace

InventoryModule::InventoryModule(RepositoryManager &repositoryManager)
{
    SPDLOG_TRACE("InventoryModule::InventoryModule");

    m_productTypesRepository = std::move(repositoryManager.getProductTypeRepository());
}

InventoryModule::~InventoryModule() { SPDLOG_TRACE("InventoryModule::~InventoryModule"); }

ResponseData InventoryModule::executeTask(const RequestData &requestData)
{
    SPDLOG_TRACE("InventoryModule::executeTask");

    if (requestData.submodule != "product-types") {
        throw ServerException(_M, "Unrecognized inventory task");
    }

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
