#include "ManagementModule.hpp"

#include "Database/RepositoryManager.hpp"
#include "ModuleUtils.hpp"

#define _M "ManagementModule"

namespace {
std::string normalizeContactType(const std::string &value)
{
    if (value == "Customer" || value == "Клієнт") {
        return "Customer";
    }
    if (value == "Client") {
        return "Client";
    }
    if (value == "Partner" || value == "Партнер") {
        return "Partner";
    }

    return value;
}

Common::Entities::Contact buildContact(const Dataset &request, const std::string &resourceId)
{
    auto type =
        ModuleUtils::getOptionalDatasetValue(request, Common::Entities::Contact::TYPE_KEY);
    if (type.empty()) {
        type = "Customer";
    } else {
        type = normalizeContactType(type);
    }

    return Common::Entities::Contact{
        .id = resourceId,
        .name = ModuleUtils::getRequiredDatasetValue(request, Common::Entities::Contact::NAME_KEY,
                                                     _M, "ManagementModule::buildContact"),
        .type = type,
        .email =
            ModuleUtils::getOptionalDatasetValue(request, Common::Entities::Contact::EMAIL_KEY),
        .phone =
            ModuleUtils::getOptionalDatasetValue(request, Common::Entities::Contact::PHONE_KEY)};
}

Common::Entities::Supplier buildSupplier(const Dataset &request, const std::string &resourceId)
{
    return Common::Entities::Supplier{
        .id = resourceId,
        .name = ModuleUtils::getRequiredDatasetValue(request, Common::Entities::Supplier::NAME_KEY,
                                                     _M, "ManagementModule::buildSupplier"),
        .number =
            ModuleUtils::getOptionalDatasetValue(request, Common::Entities::Supplier::NUMBER_KEY),
        .email =
            ModuleUtils::getOptionalDatasetValue(request, Common::Entities::Supplier::EMAIL_KEY),
        .address = ModuleUtils::getOptionalDatasetValue(request,
                                                        Common::Entities::Supplier::ADDRESS_KEY)};
}

Common::Entities::Employee buildEmployee(const Dataset &request, const std::string &resourceId)
{
    return Common::Entities::Employee{
        .id = resourceId,
        .name = ModuleUtils::getRequiredDatasetValue(request, Common::Entities::Employee::NAME_KEY,
                                                     _M, "ManagementModule::buildEmployee"),
        .number =
            ModuleUtils::getOptionalDatasetValue(request, Common::Entities::Employee::NUMBER_KEY),
        .email =
            ModuleUtils::getOptionalDatasetValue(request, Common::Entities::Employee::EMAIL_KEY),
        .address = ModuleUtils::getOptionalDatasetValue(request,
                                                        Common::Entities::Employee::ADDRESS_KEY)};
}
} // namespace

ManagementModule::ManagementModule(RepositoryManager &repositoryManager)
{
    m_contactsRepository  = std::move(repositoryManager.getContactRepository());
    m_suppliersRepository = std::move(repositoryManager.getSupplierRepository());
    m_employeesRepository = std::move(repositoryManager.getEmployeeRepository());
}

ManagementModule::~ManagementModule() = default;

ResponseData ManagementModule::executeTask(const RequestData &requestData)
{
    if (requestData.submodule == "contacts") {
        if (requestData.method == "GET") {
            return getContacts();
        }
        if (requestData.method == "POST") {
            addContact(requestData.dataset);
            return {};
        }
        if (requestData.method == "PUT") {
            editContact(requestData.dataset, requestData.resourceId);
            return {};
        }
        if (requestData.method == "DELETE") {
            deleteContact(requestData.resourceId);
            return {};
        }
        throw ServerException(_M, "Unrecognized method");
    }

    if (requestData.submodule == "suppliers") {
        if (requestData.method == "GET") {
            return getSuppliers();
        }
        if (requestData.method == "POST") {
            addSupplier(requestData.dataset);
            return {};
        }
        if (requestData.method == "PUT") {
            editSupplier(requestData.dataset, requestData.resourceId);
            return {};
        }
        if (requestData.method == "DELETE") {
            deleteSupplier(requestData.resourceId);
            return {};
        }
        throw ServerException(_M, "Unrecognized method");
    }

    if (requestData.submodule == "employees") {
        if (requestData.method == "GET") {
            return getEmployees();
        }
        if (requestData.method == "POST") {
            addEmployee(requestData.dataset);
            return {};
        }
        if (requestData.method == "PUT") {
            editEmployee(requestData.dataset, requestData.resourceId);
            return {};
        }
        if (requestData.method == "DELETE") {
            deleteEmployee(requestData.resourceId);
            return {};
        }
        throw ServerException(_M, "Unrecognized method");
    }

    throw ServerException(_M, "Unrecognized management task");
}

ResponseData ManagementModule::getContacts() const
{
    ResponseData response;
    response.dataset[Common::Entities::Contact::ID_KEY]    = {};
    response.dataset[Common::Entities::Contact::NAME_KEY]  = {};
    response.dataset[Common::Entities::Contact::TYPE_KEY]  = {};
    response.dataset[Common::Entities::Contact::EMAIL_KEY] = {};
    response.dataset[Common::Entities::Contact::PHONE_KEY] = {};

    for (const auto &contact : m_contactsRepository->getAll()) {
        response.dataset[Common::Entities::Contact::ID_KEY].emplace_back(contact.id);
        response.dataset[Common::Entities::Contact::NAME_KEY].emplace_back(contact.name);
        response.dataset[Common::Entities::Contact::TYPE_KEY].emplace_back(contact.type);
        response.dataset[Common::Entities::Contact::EMAIL_KEY].emplace_back(contact.email);
        response.dataset[Common::Entities::Contact::PHONE_KEY].emplace_back(contact.phone);
    }

    return response;
}

ResponseData ManagementModule::getSuppliers() const
{
    ResponseData response;
    response.dataset[Common::Entities::Supplier::ID_KEY]      = {};
    response.dataset[Common::Entities::Supplier::NAME_KEY]    = {};
    response.dataset[Common::Entities::Supplier::NUMBER_KEY]  = {};
    response.dataset[Common::Entities::Supplier::EMAIL_KEY]   = {};
    response.dataset[Common::Entities::Supplier::ADDRESS_KEY] = {};

    for (const auto &supplier : m_suppliersRepository->getAll()) {
        response.dataset[Common::Entities::Supplier::ID_KEY].emplace_back(supplier.id);
        response.dataset[Common::Entities::Supplier::NAME_KEY].emplace_back(supplier.name);
        response.dataset[Common::Entities::Supplier::NUMBER_KEY].emplace_back(supplier.number);
        response.dataset[Common::Entities::Supplier::EMAIL_KEY].emplace_back(supplier.email);
        response.dataset[Common::Entities::Supplier::ADDRESS_KEY].emplace_back(supplier.address);
    }

    return response;
}

ResponseData ManagementModule::getEmployees() const
{
    ResponseData response;
    response.dataset[Common::Entities::Employee::ID_KEY]      = {};
    response.dataset[Common::Entities::Employee::NAME_KEY]    = {};
    response.dataset[Common::Entities::Employee::NUMBER_KEY]  = {};
    response.dataset[Common::Entities::Employee::EMAIL_KEY]   = {};
    response.dataset[Common::Entities::Employee::ADDRESS_KEY] = {};

    for (const auto &employee : m_employeesRepository->getAll()) {
        response.dataset[Common::Entities::Employee::ID_KEY].emplace_back(employee.id);
        response.dataset[Common::Entities::Employee::NAME_KEY].emplace_back(employee.name);
        response.dataset[Common::Entities::Employee::NUMBER_KEY].emplace_back(employee.number);
        response.dataset[Common::Entities::Employee::EMAIL_KEY].emplace_back(employee.email);
        response.dataset[Common::Entities::Employee::ADDRESS_KEY].emplace_back(employee.address);
    }

    return response;
}

void ManagementModule::addContact(const Dataset &request) const
{
    m_contactsRepository->add(buildContact(request, ""));
}

void ManagementModule::editContact(const Dataset &request, const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Contact");
    ModuleUtils::ensureEntityExists(m_contactsRepository, Common::Entities::Contact::ID_KEY,
                                    resourceId, _M, "Contact does not exist");
    m_contactsRepository->update(buildContact(request, resourceId));
}

void ManagementModule::deleteContact(const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Contact");
    ModuleUtils::ensureEntityExists(m_contactsRepository, Common::Entities::Contact::ID_KEY,
                                    resourceId, _M, "Contact does not exist");
    m_contactsRepository->deleteResource(resourceId);
}

void ManagementModule::addSupplier(const Dataset &request) const
{
    m_suppliersRepository->add(buildSupplier(request, ""));
}

void ManagementModule::editSupplier(const Dataset &request, const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Supplier");
    ModuleUtils::ensureEntityExists(m_suppliersRepository, Common::Entities::Supplier::ID_KEY,
                                    resourceId, _M, "Supplier does not exist");
    m_suppliersRepository->update(buildSupplier(request, resourceId));
}

void ManagementModule::deleteSupplier(const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Supplier");
    ModuleUtils::ensureEntityExists(m_suppliersRepository, Common::Entities::Supplier::ID_KEY,
                                    resourceId, _M, "Supplier does not exist");
    m_suppliersRepository->deleteResource(resourceId);
}

void ManagementModule::addEmployee(const Dataset &request) const
{
    m_employeesRepository->add(buildEmployee(request, ""));
}

void ManagementModule::editEmployee(const Dataset &request, const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Employee");
    ModuleUtils::ensureEntityExists(m_employeesRepository, Common::Entities::Employee::ID_KEY,
                                    resourceId, _M, "Employee does not exist");
    m_employeesRepository->update(buildEmployee(request, resourceId));
}

void ManagementModule::deleteEmployee(const std::string &resourceId) const
{
    ModuleUtils::ensureResourceId(resourceId, _M, "Employee");
    ModuleUtils::ensureEntityExists(m_employeesRepository, Common::Entities::Employee::ID_KEY,
                                    resourceId, _M, "Employee does not exist");
    m_employeesRepository->deleteResource(resourceId);
}
