## 1) Goal and context

- Ticket/branch: `feature/full-mvp-development`
- Problem statement:
  - The desktop now routes every menu action, but most non-inventory modules still land on truthful placeholders.
  - The user wants the missing recommended implementation points advanced into a working ERP baseline.
  - The system also needs multi-language support (`en`, `ua`) configurable from Settings.
- Intended outcome:
  - convert schema-backed planned modules into real end-to-end slices where repo evidence supports them
  - add persisted Settings with language selection and startup language loading
  - keep the server API resource-oriented and usable beyond the Qt desktop client

## 2) Scope

- In scope:
  - startup/application settings persistence using `QSettings`
  - `en` and `ua` UI language support with a Settings surface
  - `management` CRUD for `Contact`, `Employee`, `Supplier`
  - `purchase` CRUD for `PurchaseOrder` and `PurchaseOrderRecord`
  - `sales` CRUD for `SaleOrder` and `SalesOrderRecord`
  - `logs` read-only browsing plus mutation audit-event writes where feasible
  - `analytics` summary endpoints/views grounded in real purchase/sales/inventory data
  - client/server/shared/docs/tests/diagrams/status updates for the above
- Out of scope:
  - `Company` module, because there is no shared entity or DB table yet
  - a packaging/publish pipeline beyond current repo reality
  - speculative live language hot-switch if startup/restart-based switching is the safer implementation

## 3) Current behavior (baseline)

- Implemented end-to-end today:
  - login, users CRUD, roles CRUD
  - inventory ProductType CRUD
  - inventory stock tracking
  - full desktop menu routing, but many tabs are still status views
- Current server limitation:
  - `BusinessLogic` still maps `purchase`, `sales`, `management`, `analytics`, and `logs` to `NotImplementedModule`
- Current client limitation:
  - no `QTranslator`, `QSettings`, `.ts/.qm`, or real Settings editor
- Relevant files/functions:
  - `_server/src/BusinessLogic/BusinessLogic.cpp`
  - `_server/src/Database/RepositoryManager.*`
  - `_client/src/main.cpp`
  - `_client/src/Ui/MainWindow.*`
  - `_client/src/Network/ApiManager.*`
  - `scripts/create_db.sql`
  - `_common/include/common/Endpoints.hpp`

## 4) Target behavior (DoD)

- [x] Build succeeds.
- [x] Relevant tests pass.
- [x] Settings tab becomes real and persists at least UI language.
- [x] UI supports `en` and `ua`; selected language is applied on startup.
- [x] Management module exposes CRUD for contacts/customers, employees, and suppliers.
- [x] Purchase module exposes CRUD for orders and records, with a coherent goods-receipt operation.
- [x] Sales module exposes CRUD for orders and records, with basic export support on the desktop side.
- [x] Logs module exposes read-only browsing of audit data written by real mutations.
- [x] Analytics module exposes at least one sales summary and one inventory summary based on real data.
- [x] Desktop views exist for the implemented modules and replace placeholder landing views.
- [x] `docs/Implementation_Status.md` and relevant requirements/design docs are updated truthfully.
- [x] Compatibility expectations and remaining gaps are documented.

## 5) Files to touch

| File/Dir | Why |
| --- | --- |
| `_common/include/common/Endpoints.hpp` | add new resource-oriented endpoints |
| `_server/src/BusinessLogic/` | add real modules for management/purchase/sales/logs/analytics |
| `_server/src/Database/` | add repositories for schema-backed entities and logging support |
| `_server/tests/` | add unit/component/contract/repository coverage for new slices |
| `_client/src/main.cpp` | load settings + translators at startup |
| `_client/src/Network/` | add API methods/signals/handlers for new resources |
| `_client/src/Ui/Models/` | add models for management/purchase/sales/logs/analytics/settings |
| `_client/src/Ui/ViewModels/` | add presentation logic and display mapping |
| `_client/src/Ui/Views/` | add real views and replace placeholders |
| `_client/tests/` | extend mocks and Qt tests |
| `_client/CMakeLists.txt` | compile new client sources and translation assets |
| `docs/` | update requirements/status/diagrams for the new slices |

## 6) Interfaces and compatibility

- External interfaces impacted:
  - HTTP endpoints under `/api/management/*`, `/api/purchase/*`, `/api/sales/*`, `/api/logs/*`, `/api/analytics/*`
  - desktop settings persistence and startup behavior
- Backward compatibility:
  - additive at the API level
  - existing users/inventory flows must remain intact
  - Settings should keep environment overrides as higher priority than persisted defaults
- Migration notes:
  - use existing schema-backed tables where possible
  - if extra receipt/log support needs schema expansion, document it explicitly and keep it additive

## 7) Approach (implementation steps)

- [x] Add settings/i18n foundation: `QSettings`, translation loading, EN/UA assets, and a real Settings view.
- [x] Add repositories and server module for management master data (`Contact`, `Employee`, `Supplier`).
- [x] Add client API/model/viewmodel/view support for management master data and wire menu actions to real views.
- [x] Add repositories and server module for purchase orders/records plus a receive-posting operation.
- [x] Add purchase client workflow and tests.
- [x] Add repositories and server module for sales orders/records plus desktop export.
- [x] Add sales client workflow and tests.
- [x] Add audit log persistence + read-only logs module and client view.
- [x] Add analytics summaries and client dashboard views.
- [x] Update docs/status/diagrams and run full validation.

## 8) Risk review

| Risk | Impact | Mitigation/test |
| --- | --- | --- |
| Scope is broad and crosses `_common`, `_server`, `_client`, docs, and tests | Regression risk | stage by dependency order and keep each slice vertically complete |
| Live language switching causes stale texts/widgets | Partial translation or broken UI | prefer startup/restart application of language unless runtime switch becomes safely testable |
| Purchase/Sales record routing clashes with current `/api/<module>/<submodule>/<id>` transport shape | awkward or unstable contracts | use resource-oriented separate submodules rather than deep nested routes |
| Audit logging lacks authenticated actor context | weak log usefulness | log entity/user references where present and document remaining auth/session gap truthfully |
| Reusing seeded IDs for employees/users creates brittle assumptions | data-integrity bugs | validate references explicitly and fetch lookup data from real endpoints |
| Large client UI expansion causes sluggish tab startup | poor UX | keep lazy tab creation/fetching and reuse view instances per module |

## 9) Verification plan

- `python3 build.py tests --no-clean`
- `ctest --test-dir build --output-on-failure --verbose`
- `python3 build.py docs`
- `git diff --check`
- targeted UI tests for the new views and settings/i18n bootstrap path

## 10) Progress log

- [17:29] Reconfirmed repo truth: schema-backed entities exist for management/purchase/sales/logs, but only users/inventory are implemented.
- [17:31] Confirmed client has no real settings persistence or translation infrastructure yet.
- [18:08] Completed the main ERP baseline: management, supplier catalog, purchase, sales, logs, analytics, and settings/i18n are now wired end-to-end.
- [18:15] Added client-side ERP model coverage and expanded API mock coverage for the new resource families.
- [18:25] Full validation passed: `python3 build.py tests --no-clean`, `python3 build.py smoke`, `python3 build.py smoke-gui`, `python3 build.py docs`, docs links, and `git diff --check`.

## 11) Final review

- What changed vs plan:
  - The intended scope was delivered as an ERP baseline, but the repo still does not support companies, incoming invoices, barcode labels, or richer invoice export formats. Those remain documented gaps rather than invented implementations.
- What was verified:
  - full build/test path
  - runtime inventory smoke
  - offscreen GUI startup smoke
  - docs link check
  - PlantUML render + Doxygen docs build
- Remaining risks/follow-ups:
  - no interactive end-to-end GUI automation for purchase/sales/management
  - no company model/schema yet
  - no incoming invoice/barcode/advanced export pipeline
  - no packaged release format beyond archived binaries
