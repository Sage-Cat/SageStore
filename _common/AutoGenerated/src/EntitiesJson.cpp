#include "common/Entities/Contact.hpp"
#include "common/Entities/ContactInfo.hpp"
#include "common/Entities/Employee.hpp"
#include "common/Entities/Inventory.hpp"
#include "common/Entities/Log.hpp"
#include "common/Entities/ProductInfo.hpp"
#include "common/Entities/ProductType.hpp"
#include "common/Entities/PurchaseOrder.hpp"
#include "common/Entities/PurchaseOrderRecord.hpp"
#include "common/Entities/Role.hpp"
#include "common/Entities/SaleOrder.hpp"
#include "common/Entities/SalesOrderRecord.hpp"
#include "common/Entities/Supplier.hpp"
#include "common/Entities/SuppliersProductInfo.hpp"
#include "common/Entities/User.hpp"

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace std;

namespace Common {
namespace Entities {

// --- Role ---
void to_json(json &j, const Role &u) { j = json{{Role::ID_KEY, u.id}, {Role::NAME_KEY, u.name}}; }

void from_json(const json &j, Role &u)
{
    j.at(Role::ID_KEY).get_to(u.id);
    j.at(Role::NAME_KEY).get_to(u.name);
}

// --- User ---
void to_json(json &j, const User &u)
{
    j = json{{User::ID_KEY, u.id},
             {User::USERNAME_KEY, u.username},
             {User::PASSWORD_KEY, u.password},
             {User::ROLE_ID_KEY, u.roleId}};
}

void from_json(const json &j, User &u)
{
    j.at(User::ID_KEY).get_to(u.id);
    j.at(User::USERNAME_KEY).get_to(u.username);
    j.at(User::PASSWORD_KEY).get_to(u.password);
    j.at(User::ROLE_ID_KEY).get_to(u.roleId);
}

// --- Log ---
void to_json(json &j, const Log &u)
{
    j = json{{Log::ID_KEY, u.id},
             {Log::USER_ID_KEY, u.userId},
             {Log::ACTION_KEY, u.action},
             {Log::TIMESTAMP_KEY, u.timestamp}};
}

void from_json(const json &j, Log &u)
{
    j.at(Log::ID_KEY).get_to(u.id);
    j.at(Log::USER_ID_KEY).get_to(u.userId);
    j.at(Log::ACTION_KEY).get_to(u.action);
    j.at(Log::TIMESTAMP_KEY).get_to(u.timestamp);
}

// --- Contact ---
void to_json(json &j, const Contact &u)
{
    j = json{{Contact::ID_KEY, u.id},
             {Contact::NAME_KEY, u.name},
             {Contact::TYPE_KEY, u.type},
             {Contact::EMAIL_KEY, u.email},
             {Contact::PHONE_KEY, u.phone}};
}

void from_json(const json &j, Contact &u)
{
    j.at(Contact::ID_KEY).get_to(u.id);
    j.at(Contact::NAME_KEY).get_to(u.name);
    j.at(Contact::TYPE_KEY).get_to(u.type);
    j.at(Contact::EMAIL_KEY).get_to(u.email);
    j.at(Contact::PHONE_KEY).get_to(u.phone);
}

// --- ContactInfo ---
void to_json(json &j, const ContactInfo &u)
{
    j = json{{ContactInfo::ID_KEY, u.id},
             {ContactInfo::CONTACT_ID_KEY, u.contactID},
             {ContactInfo::NAME_KEY, u.name},
             {ContactInfo::VALUE_KEY, u.value}};
}

void from_json(const json &j, ContactInfo &u)
{
    j.at(ContactInfo::ID_KEY).get_to(u.id);
    j.at(ContactInfo::CONTACT_ID_KEY).get_to(u.contactID);
    j.at(ContactInfo::NAME_KEY).get_to(u.name);
    j.at(ContactInfo::VALUE_KEY).get_to(u.value);
}

// --- Employee ---
void to_json(json &j, const Employee &u)
{
    j = json{{Employee::ID_KEY, u.id},
             {Employee::NAME_KEY, u.name},
             {Employee::NUMBER_KEY, u.number},
             {Employee::EMAIL_KEY, u.email},
             {Employee::ADDRESS_KEY, u.address}};
}

void from_json(const json &j, Employee &u)
{
    j.at(Employee::ID_KEY).get_to(u.id);
    j.at(Employee::NAME_KEY).get_to(u.name);
    j.at(Employee::NUMBER_KEY).get_to(u.number);
    j.at(Employee::EMAIL_KEY).get_to(u.email);
    j.at(Employee::ADDRESS_KEY).get_to(u.address);
}

// --- Supplier ---
void to_json(json &j, const Supplier &u)
{
    j = json{{Supplier::ID_KEY, u.id},
             {Supplier::NAME_KEY, u.name},
             {Supplier::NUMBER_KEY, u.number},
             {Supplier::EMAIL_KEY, u.email},
             {Supplier::ADDRESS_KEY, u.address}};
}

void from_json(const json &j, Supplier &u)
{
    j.at(Supplier::ID_KEY).get_to(u.id);
    j.at(Supplier::NAME_KEY).get_to(u.name);
    j.at(Supplier::NUMBER_KEY).get_to(u.number);
    j.at(Supplier::EMAIL_KEY).get_to(u.email);
    j.at(Supplier::ADDRESS_KEY).get_to(u.address);
}

// --- SuppliersProductInfo ---
void to_json(json &j, const SuppliersProductInfo &u)
{
    j = json{{SuppliersProductInfo::ID_KEY, u.id},
             {SuppliersProductInfo::SUPPLIER_ID_KEY, u.supplierID},
             {SuppliersProductInfo::PRODUCT_TYPE_ID_KEY, u.productTypeId},
             {SuppliersProductInfo::CODE_KEY, u.code}};
}

void from_json(const json &j, SuppliersProductInfo &u)
{
    j.at(SuppliersProductInfo::ID_KEY).get_to(u.id);
    j.at(SuppliersProductInfo::SUPPLIER_ID_KEY).get_to(u.supplierID);
    j.at(SuppliersProductInfo::PRODUCT_TYPE_ID_KEY).get_to(u.productTypeId);
    j.at(SuppliersProductInfo::CODE_KEY).get_to(u.code);
}

// --- ProductInfo ---
void to_json(json &j, const ProductInfo &u)
{
    j = json{{ProductInfo::ID_KEY, u.id},
             {ProductInfo::PRODUCT_TYPE_ID_KEY, u.productTypeId},
             {ProductInfo::NAME_KEY, u.name},
             {ProductInfo::VALUE_KEY, u.value}};
}

void from_json(const json &j, ProductInfo &u)
{
    j.at(ProductInfo::ID_KEY).get_to(u.id);
    j.at(ProductInfo::PRODUCT_TYPE_ID_KEY).get_to(u.productTypeId);
    j.at(ProductInfo::NAME_KEY).get_to(u.name);
    j.at(ProductInfo::VALUE_KEY).get_to(u.value);
}

// --- ProductType ---
void to_json(json &j, const ProductType &u)
{
    j = json{{ProductType::ID_KEY, u.id},
             {ProductType::CODE_KEY, u.code},
             {ProductType::BARCODE_KEY, u.barcode},
             {ProductType::NAME_KEY, u.name},
             {ProductType::DESCRIPTION_KEY, u.description},
             {ProductType::LAST_PRICE_KEY, u.lastPrice},
             {ProductType::UNIT_KEY, u.unit},
             {ProductType::IS_IMPORTED_KEY, u.isImported}};
}

void from_json(const json &j, ProductType &u)
{
    j.at(ProductType::ID_KEY).get_to(u.id);
    j.at(ProductType::CODE_KEY).get_to(u.code);
    j.at(ProductType::BARCODE_KEY).get_to(u.barcode);
    j.at(ProductType::NAME_KEY).get_to(u.name);
    j.at(ProductType::DESCRIPTION_KEY).get_to(u.description);
    j.at(ProductType::LAST_PRICE_KEY).get_to(u.lastPrice);
    j.at(ProductType::UNIT_KEY).get_to(u.unit);
    j.at(ProductType::IS_IMPORTED_KEY).get_to(u.isImported);
}

// --- Inventory ---
void to_json(json &j, const Inventory &u)
{
    j = json{{Inventory::ID_KEY, u.id},
             {Inventory::PRODUCT_TYPE_ID_KEY, u.productTypeId},
             {Inventory::QUANTITY_AVAILABLE_KEY, u.quantityAvailable},
             {Inventory::EMPLOYEE_ID_KEY, u.employeeId}};
}

void from_json(const json &j, Inventory &u)
{
    j.at(Inventory::ID_KEY).get_to(u.id);
    j.at(Inventory::PRODUCT_TYPE_ID_KEY).get_to(u.productTypeId);
    j.at(Inventory::QUANTITY_AVAILABLE_KEY).get_to(u.quantityAvailable);
    j.at(Inventory::EMPLOYEE_ID_KEY).get_to(u.employeeId);
}

// --- SaleOrder ---
void to_json(json &j, const SaleOrder &u)
{
    j = json{{SaleOrder::ID_KEY, u.id},
             {SaleOrder::DATE_KEY, u.date},
             {SaleOrder::USER_ID_KEY, u.userId},
             {SaleOrder::CONTACT_ID_KEY, u.contactId},
             {SaleOrder::EMPLOYEE_ID_KEY, u.employeeId},
             {SaleOrder::STATUS_KEY, u.status}};
}

void from_json(const json &j, SaleOrder &u)
{
    j.at(SaleOrder::ID_KEY).get_to(u.id);
    j.at(SaleOrder::DATE_KEY).get_to(u.date);
    j.at(SaleOrder::USER_ID_KEY).get_to(u.userId);
    j.at(SaleOrder::CONTACT_ID_KEY).get_to(u.contactId);
    j.at(SaleOrder::EMPLOYEE_ID_KEY).get_to(u.employeeId);
    j.at(SaleOrder::STATUS_KEY).get_to(u.status);
}

// --- SalesOrderRecord ---
void to_json(json &j, const SalesOrderRecord &u)
{
    j = json{{SalesOrderRecord::ID_KEY, u.id},
             {SalesOrderRecord::ORDER_ID_KEY, u.orderId},
             {SalesOrderRecord::PRODUCT_TYPE_ID_KEY, u.productTypeId},
             {SalesOrderRecord::QUANTITY_KEY, u.quantity},
             {SalesOrderRecord::PRICE_KEY, u.price}};
}

void from_json(const json &j, SalesOrderRecord &u)
{
    j.at(SalesOrderRecord::ID_KEY).get_to(u.id);
    j.at(SalesOrderRecord::ORDER_ID_KEY).get_to(u.orderId);
    j.at(SalesOrderRecord::PRODUCT_TYPE_ID_KEY).get_to(u.productTypeId);
    j.at(SalesOrderRecord::QUANTITY_KEY).get_to(u.quantity);
    j.at(SalesOrderRecord::PRICE_KEY).get_to(u.price);
}

// --- PurchaseOrder ---
void to_json(json &j, const PurchaseOrder &u)
{
    j = json{{PurchaseOrder::ID_KEY, u.id},
             {PurchaseOrder::DATE_KEY, u.date},
             {PurchaseOrder::USER_ID_KEY, u.userId},
             {PurchaseOrder::SUPPLIER_ID_KEY, u.supplierId},
             {PurchaseOrder::STATUS_KEY, u.status}};
}

void from_json(const json &j, PurchaseOrder &u)
{
    j.at(PurchaseOrder::ID_KEY).get_to(u.id);
    j.at(PurchaseOrder::DATE_KEY).get_to(u.date);
    j.at(PurchaseOrder::USER_ID_KEY).get_to(u.userId);
    j.at(PurchaseOrder::SUPPLIER_ID_KEY).get_to(u.supplierId);
    j.at(PurchaseOrder::STATUS_KEY).get_to(u.status);
}

// --- PurchaseOrderRecord ---
void to_json(json &j, const PurchaseOrderRecord &u)
{
    j = json{{PurchaseOrderRecord::ID_KEY, u.id},
             {PurchaseOrderRecord::ORDER_ID_KEY, u.orderId},
             {PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY, u.productTypeId},
             {PurchaseOrderRecord::QUANTITY_KEY, u.quantity},
             {PurchaseOrderRecord::PRICE_KEY, u.price}};
}

void from_json(const json &j, PurchaseOrderRecord &u)
{
    j.at(PurchaseOrderRecord::ID_KEY).get_to(u.id);
    j.at(PurchaseOrderRecord::ORDER_ID_KEY).get_to(u.orderId);
    j.at(PurchaseOrderRecord::PRODUCT_TYPE_ID_KEY).get_to(u.productTypeId);
    j.at(PurchaseOrderRecord::QUANTITY_KEY).get_to(u.quantity);
    j.at(PurchaseOrderRecord::PRICE_KEY).get_to(u.price);
}

} // namespace Entities
} // namespace Common
