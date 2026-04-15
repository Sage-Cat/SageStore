# SageStore Implementation Status

## Snapshot Date

- 2026-04-15

## Slice Artifacts

- [Requirements baseline and reconciliation](Requirements_Baseline.md)
- [Future architecture and design roadmap](Future_Architecture_and_Design_Roadmap.md)
- [ERP readiness assessment](ERP_Readiness_Assessment.md)
- [ERP testing strategy](ERP_Testing_Strategy.md)
- [Inventory/ProductType requirements and gaps](inventory/ProductType_Slice_Requirements_Gap.md)
- [Inventory/ProductType release readiness](inventory/ProductType_Release_Readiness.md)
- [Deployment runbook](Deployment_Runbook.md)
- [Debug CLI design](tools/Debug_CLI_Design.md)

## Scope Matrix

| Area | Documentation Target | Implemented in Code | Tests | Gap |
| --- | --- | --- | --- | --- |
| Users: login | Defined | Implemented through `UsersModule`, `ApiManager`, `DialogManager`, and desktop login flow | Yes (`UsersModuleTest`, `BusinessLogicTest`, `ApiManagerTest`, `DialogManagerTest`) | No dedicated self-profile flow |
| Users: users CRUD | Defined | Implemented across server repositories/business logic and desktop MVVM/UI | Yes (server unit/component/integration + client unit/UI) | No live GUI/system CRUD coverage yet |
| Users: roles CRUD | Defined | Implemented across server, API, and desktop roles view | Yes (`UsersModuleTest`, `ApiManagerTest`, `RepositoryIntegrationTest`, `RolesViewTest`, `MainWindowTest`) | No material functional gap |
| Settings and localization | Implicitly required by branch goal | Implemented with `AppSettings`, `SettingsView`, startup loading in `main.cpp`, and EN/UA support through `TsTranslator` | Yes (`SettingsViewTest`, `MainWindowTest`) | Language switch is restart-applied, not live hot-swap |
| Desktop module navigation | Defined | Every menu action opens a real workspace tab or implemented slice; lazy tab creation avoids upfront GUI load | Yes (`MainWindowTest`) | No unsupported menu actions remain |
| Management: contacts/customers | Defined | Implemented via `/api/management/contacts`, `ManagementModule`, repositories, and `ManagementView` | Yes (server unit/component/integration + `ErpModelsTest` + `ManagementViewTest`) | No company support yet |
| Management: suppliers | Defined | Implemented via `/api/management/suppliers`, `ManagementModule`, repositories, and `ManagementView` | Yes (server unit/component/integration + `ErpModelsTest` + `ManagementViewTest`) | No company support yet |
| Management: employees | Defined | Implemented via `/api/management/employees`, `ManagementModule`, repositories, and `ManagementView` | Yes (server unit/component/integration + `ErpModelsTest` + `ManagementViewTest`) | No employee-history view |
| Inventory: ProductType CRUD | Defined | Implemented end-to-end across `_common`, `_server`, `_client`, and smoke/runtime paths | Yes (server unit + component + repository integration + HTTP contract + client unit + Qt workflow) | No barcode PDF/label generation yet |
| Inventory: stock tracking | Defined | Implemented via `InventoryModule`, `InventoryRepository`, `Stocks*`, and smoke validation | Yes (server unit + component + repository integration + HTTP contract + client unit + Qt workflow + smoke) | No company/storage scope or stock-movement ledger |
| Inventory: supplier product base | Defined | Implemented via `SuppliersProductInfoRepository`, `InventoryModule`, `SupplierCatalog*`, and desktop CSV import | Yes (server unit + repository integration + HTTP contract + `ApiManagerTest` + `ErpModelsTest` + `SupplierCatalogViewTest`) | No XML/XLSX import or auto-create ProductTypes from invoices |
| Purchase: orders and order records | Defined | Implemented via `/api/purchase/orders` and `/api/purchase/order-records`, real repositories/modules, and `PurchaseView` | Yes (server unit + component + integration + `ApiManagerTest` + `ErpModelsTest` + `PurchaseViewTest`) | No incoming invoice attachment workflow |
| Purchase: goods receipts | Defined | Implemented via `/api/purchase/receipts`; receipt posting updates stock and marks orders received | Yes (`ErpModulesTest`, `HttpContractTest`, `ApiManagerTest`, `ErpModelsTest`) | No invoice file upload/registration flow |
| Sales: orders and order records | Defined | Implemented via `/api/sales/orders` and `/api/sales/order-records`, repositories/modules, and `SalesView` | Yes (server unit + component + integration + `ApiManagerTest` + `ErpModelsTest` + `SalesViewTest`) | No server-side reservation/posting/cancel lifecycle or stock interaction |
| Sales: invoice export | Defined | Implemented as simple desktop text export/preview in `SalesView` | Yes (`SalesViewTest`, `Component_FullstackSalesInvoiceExportTest`, `Integration_FullstackWorkflowTest`) | No HTML/PDF/XML export pipeline or print flow |
| Logs | Defined | Implemented with `LogRepository`, `LogsModule`, audit writes from `BusinessLogic`, and desktop browsing in `LogsView` | Yes (`BusinessLogicTest`, `RepositoryIntegrationTest`, `HttpContractTest`, `ErpModulesTest`, `ErpModelsTest`) | No richer filtering/export/history partitioning |
| Analytics | Defined | Implemented via read-only sales and inventory summary endpoints plus a richer `AnalyticsView` dashboard with profit, margin, and goods-flow visuals | Yes (`BusinessLogicTest`, `HttpContractTest`, `ErpModulesTest`, `ApiManagerTest`, `ErpModelsTest`, `AnalyticsViewTest`) | No historical trend reporting or drill-down analytics |
| Companies | Defined in broader target docs | Not implemented | No | High |

## Architecture Baseline

- Client:
  - `MainWindow` lazily hosts users, roles, management, purchasing, sales, inventory, logs, analytics, supplier catalog, and settings workspaces.
  - `ApiManager` stays resource-oriented and reusable beyond the current desktop client.
  - `AppSettings` persists desktop defaults locally; environment variables remain higher priority for runtime server settings.
  - `TsTranslator` loads Ukrainian UI translations at startup when the saved language is `ua`; English remains the source language.
- Server:
  - `HttpServer/HttpTransaction` routes into `BusinessLogic`.
  - `BusinessLogic` now dispatches real modules for `users`, `system`, `inventory`, `management`, `purchase`, `sales`, `logs`, and `analytics`.
  - `system/health` provides a neutral readiness probe for automation and deployment checks.
  - Non-GET non-logs mutations write audit entries through `LogRepository`.
- Data access:
  - `RepositoryManager` now serves repositories for users, roles, inventory, product types, contacts, suppliers, employees, supplier product mappings, purchase orders, purchase order records, sale orders, sale order records, and logs over SQLite.

## Quality/Process Baseline

- Jenkins runs checkout, clean, Conan install, `python3 build.py docs`, CMake configure/build, `ctest`, the non-interactive inventory smoke path, and the offscreen GUI startup smoke path.
- `python3 build.py docs` validates markdown links, renders PlantUML when available, and runs Doxygen.
- `python3 build.py smoke` runs the isolated inventory/API smoke path.
- `python3 build.py smoke-gui` runs an offscreen client startup smoke against the real server.
- `python3 build.py regression` now runs the labeled offscreen regression lane plus both smoke scripts under forced headless settings.
- `SageStoreDebugCli` now provides a machine-readable debug surface for Codex-style runs: direct server requests, optional local server spawn, offscreen client startup/login, covered UI actions, widget-tree state dumps, screenshots, and automatic failure artifacts.
- `ctest` now includes Qt-driven client/server workflow tests that launch the real server binary and drive inventory CRUD, management contact creation, purchase receipt posting, sales invoice preview/export, and broad workspace loading through real desktop widgets.
- Offscreen runtime validation now has hard gates for:
  - error-level log lines in smoke/debug-cli runs
  - screenshot decode/dimension/non-blank checks
  - state-dump sanity for captured workspaces
  - labeled/time-bounded/serialized UI fullstack execution

## Test Coverage Focus (Current)

- Users and roles business logic plus desktop management flows.
- Inventory ProductType, stock, and supplier catalog contracts and repository behavior.
- Management, purchase, sales, logs, and analytics business modules.
- Purchase receipt posting into stock.
- API contract coverage for management, supplier catalog, purchase, sales, logs, and analytics.
- Desktop model coverage for management, supplier catalog, purchase, sales, logs, analytics, and settings.
- Direct desktop Qt workflow coverage for users, management, supplier catalog, purchase, sales, analytics, inventory, and roles.
- Interactive client/server workflow coverage now exists through:
  - `System_FullstackInventoryWorkflowTest`
  - `Component_FullstackMainWindowWorkflowTest`
  - `Component_FullstackSalesInvoiceExportTest`
  - `Integration_FullstackWorkflowTest`
  - `System_DebugCliScenarioTest`
- Debug CLI coverage now validates:
  - `describe` command contract
  - offscreen login + major workspace navigation + screenshot/state artifact capture
  - screenshot and state artifact quality, not just file existence
  - clean runtime logs for the Debug CLI regression scenarios
  - selector-failure JSON reporting plus automatic failure artifacts
- Gap:
  - no live auth failure/registration workflow coverage yet
  - no live GUI/system CRUD coverage yet for users/roles, suppliers, employees, purchase-order editing, or sales posting/cancel flows
  - no installed-binary/package validation yet

## Immediate Priorities and Roadmap Bridge

This page remains the current-state source of truth. For the longer-horizon phase model,
architecture direction, and design-system guidance, see
`docs/Future_Architecture_and_Design_Roadmap.md`.

### 30 Days

1. Expand live GUI/system coverage from load/preview slices into write-heavy auth, users/roles, management, purchase-order, and sales-fulfillment workflows.
2. Define the first honest packaged desktop distribution format beyond archived binaries.
3. Add health/migration/backup basics for installable deployments.

### 60 Days

1. Resolve and implement company-aware storage and management if the business fields are finalized.
2. Define incoming invoice registration/upload contracts and evidence-backed import formats.
3. Replace mutable-balance-only stock semantics with explicit stock-movement rules.

### 90 Days

1. Implement barcode/label generation and printing with a justified rendering/output approach.
2. Expand release automation from archived artifacts to an installable/packageable release.
3. Stabilize generic catalog extensibility so autoparts remains an optional domain pack, not a core-schema fork.

Longer-horizon development should then continue in this ERP order:

1. master data and inventory truth
2. procurement/document processing completion
3. sales fulfillment and richer document outputs
4. analytics, operability, and broader platform extension

## Acceptance Criteria For Resumed Development

- Every new module or materially expanded slice must include:
  - business module + repository implementation
  - endpoint wiring and serializer compatibility
  - at least one server unit test and one integration/contract test
  - desktop/client coverage where a desktop flow exists
  - updated PlantUML and status/requirements docs
- Remaining major completion gates for the current ERP baseline are:
  - companies support
  - company-aware storage and stock truth
  - stock ledger and sales stock-posting policy
  - incoming invoices and barcode/label workflows
  - richer sales export formats
  - broader interactive end-to-end GUI/system validation
  - a real deployable package/release path
