# SageStore Implementation Status

## Snapshot Date

- 2026-03-13

## Slice Artifacts

- [Requirements baseline and reconciliation](Requirements_Baseline.md)
- [Inventory/ProductType requirements and gaps](inventory/ProductType_Slice_Requirements_Gap.md)
- [Inventory/ProductType release readiness](inventory/ProductType_Release_Readiness.md)
- [Deployment runbook](Deployment_Runbook.md)

## Scope Matrix

| Area | Documentation Target | Implemented in Code | Tests | Gap |
| --- | --- | --- | --- | --- |
| Users: login | Defined | Implemented through `UsersModule`, `ApiManager`, `DialogManager`, and desktop login flow | Yes (`UsersModuleTest`, `BusinessLogicTest`, `ApiManagerTest`, `DialogManagerTest`) | No dedicated self-profile flow |
| Users: users CRUD | Defined | Implemented across server repositories/business logic and desktop MVVM/UI | Yes (server unit/component/integration + client unit/UI) | Broader live GUI/system coverage |
| Users: roles CRUD | Defined | Implemented across server, API, and desktop roles view | Yes (`UsersModuleTest`, `ApiManagerTest`, `RepositoryIntegrationTest`, `RolesViewTest`, `MainWindowTest`) | No material functional gap |
| Settings and localization | Implicitly required by branch goal | Implemented with `AppSettings`, `SettingsView`, startup loading in `main.cpp`, and EN/UA support through `TsTranslator` | Yes (`SettingsViewTest`, `MainWindowTest`) | Language switch is restart-applied, not live hot-swap |
| Desktop module navigation | Defined | Every menu action opens a real workspace tab or implemented slice; lazy tab creation avoids upfront GUI load | Yes (`MainWindowTest`) | No unsupported menu actions remain |
| Management: contacts/customers | Defined | Implemented via `/api/management/contacts`, `ManagementModule`, repositories, and `ManagementView` | Yes (server unit/component/integration + `ErpModelsTest`) | No company support yet |
| Management: suppliers | Defined | Implemented via `/api/management/suppliers`, `ManagementModule`, repositories, and `ManagementView` | Yes (server unit/component/integration + `ErpModelsTest`) | No company support yet |
| Management: employees | Defined | Implemented via `/api/management/employees`, `ManagementModule`, repositories, and `ManagementView` | Yes (server unit/component/integration + `ErpModelsTest`) | No employee-history view |
| Inventory: ProductType CRUD | Defined | Implemented end-to-end across `_common`, `_server`, `_client`, and smoke/runtime paths | Yes (server unit + component + repository integration + HTTP contract + client unit + Qt workflow) | No barcode PDF/label generation yet |
| Inventory: stock tracking | Defined | Implemented via `InventoryModule`, `InventoryRepository`, `Stocks*`, and smoke validation | Yes (server unit + component + repository integration + HTTP contract + client unit + Qt workflow + smoke) | No multi-storage/company-aware inventory |
| Inventory: supplier product base | Defined | Implemented via `SuppliersProductInfoRepository`, `InventoryModule`, `SupplierCatalog*`, and desktop CSV import | Yes (server unit + repository integration + HTTP contract + `ApiManagerTest` + `ErpModelsTest`) | No XML/XLSX import or auto-create ProductTypes from invoices |
| Purchase: orders and order records | Defined | Implemented via `/api/purchase/orders` and `/api/purchase/order-records`, real repositories/modules, and `PurchaseView` | Yes (server unit + component + integration + `ApiManagerTest` + `ErpModelsTest`) | No incoming invoice attachment workflow |
| Purchase: goods receipts | Defined | Implemented via `/api/purchase/receipts`; receipt posting updates stock and marks orders received | Yes (`ErpModulesTest`, `HttpContractTest`, `ApiManagerTest`, `ErpModelsTest`) | No invoice file upload/registration flow |
| Sales: orders and order records | Defined | Implemented via `/api/sales/orders` and `/api/sales/order-records`, repositories/modules, and `SalesView` | Yes (server unit + component + integration + `ApiManagerTest` + `ErpModelsTest`) | No stock decrement/posting on sales yet |
| Sales: invoice export | Defined | Implemented as simple desktop text export/preview in `SalesView` | Partially (`MainWindowTest` covers workspace routing; no dedicated export test yet) | No HTML/PDF/XML export pipeline or print flow |
| Logs | Defined | Implemented with `LogRepository`, `LogsModule`, audit writes from `BusinessLogic`, and desktop browsing in `LogsView` | Yes (`BusinessLogicTest`, `RepositoryIntegrationTest`, `HttpContractTest`, `ErpModulesTest`, `ErpModelsTest`) | No richer filtering/export/history partitioning |
| Analytics | Defined | Implemented via read-only sales and inventory summary endpoints plus `AnalyticsView` | Yes (`BusinessLogicTest`, `HttpContractTest`, `ErpModulesTest`, `ApiManagerTest`, `ErpModelsTest`) | No advanced reporting or charting |
| Companies | Defined in broader target docs | Not implemented | No | High |

## Architecture Baseline

- Client:
  - `MainWindow` lazily hosts users, roles, management, purchasing, sales, inventory, logs, analytics, supplier catalog, and settings workspaces.
  - `ApiManager` stays resource-oriented and reusable beyond the current desktop client.
  - `AppSettings` persists desktop defaults locally; environment variables remain higher priority for runtime server settings.
  - `TsTranslator` loads Ukrainian UI translations at startup when the saved language is `ua`; English remains the source language.
- Server:
  - `HttpServer/HttpTransaction` routes into `BusinessLogic`.
  - `BusinessLogic` now dispatches real modules for `users`, `inventory`, `management`, `purchase`, `sales`, `logs`, and `analytics`.
  - Non-GET non-logs mutations write audit entries through `LogRepository`.
- Data access:
  - `RepositoryManager` now serves repositories for users, roles, inventory, product types, contacts, suppliers, employees, supplier product mappings, purchase orders, purchase order records, sale orders, sale order records, and logs over SQLite.

## Quality/Process Baseline

- Jenkins runs checkout, clean, Conan install, `python3 build.py docs`, CMake configure/build, `ctest`, the non-interactive inventory smoke path, and the offscreen GUI startup smoke path.
- `python3 build.py docs` validates markdown links, renders PlantUML when available, and runs Doxygen.
- `python3 build.py smoke` runs the isolated inventory/API smoke path.
- `python3 build.py smoke-gui` runs an offscreen client startup smoke against the real server.

## Test Coverage Focus (Current)

- Users and roles business logic plus desktop management flows.
- Inventory ProductType, stock, and supplier catalog contracts and repository behavior.
- Management, purchase, sales, logs, and analytics business modules.
- Purchase receipt posting into stock.
- API contract coverage for management, supplier catalog, purchase, sales, logs, and analytics.
- Desktop model coverage for management, supplier catalog, purchase, sales, logs, analytics, and settings.
- Gap:
  - no interactive full client/server GUI automation for purchase/sales/management workflows
  - no dedicated automated test for sales invoice export file contents

## 30/60/90 Plan

### 30 Days

1. Add interactive fullstack GUI/system coverage for the main ERP workspaces, not just startup smoke.
2. Add a dedicated automated test for sales invoice export output.
3. Define the first honest packaged desktop distribution format beyond archived binaries.

### 60 Days

1. Resolve and implement company-aware storage and management if the business fields are finalized.
2. Define incoming invoice registration/upload contracts and evidence-backed import formats.
3. Decide whether sales posting should update inventory quantities in the baseline workflow.

### 90 Days

1. Implement barcode/label generation and printing with a justified rendering/output approach.
2. Expand release automation from archived artifacts to an installable/packageable release.
3. Resolve registration policy versus admin-only user provisioning and implement the chosen behavior.

## Acceptance Criteria For Resumed Development

- Every new module or materially expanded slice must include:
  - business module + repository implementation
  - endpoint wiring and serializer compatibility
  - at least one server unit test and one integration/contract test
  - desktop/client coverage where a desktop flow exists
  - updated PlantUML and status/requirements docs
- Remaining major completion gates for the current ERP baseline are:
  - companies support
  - incoming invoices and barcode/label workflows
  - richer sales export formats
  - interactive end-to-end GUI/system validation
  - a real deployable package/release path
