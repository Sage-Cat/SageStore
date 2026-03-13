# SageStore Implementation Status

## Snapshot Date

- 2026-03-13

## Slice Artifacts

- [Requirements baseline and reconciliation](Requirements_Baseline.md)
- [Inventory/ProductType requirements and gaps](inventory/ProductType_Slice_Requirements_Gap.md)
- [Inventory/ProductType release readiness](inventory/ProductType_Release_Readiness.md)

## Scope Matrix

| Area | Documentation Target | Implemented in Code | Tests | Gap |
| --- | --- | --- | --- | --- |
| Users: login | Defined | Implemented (`_server/src/BusinessLogic/UsersModule.cpp`, `_client/src/Network/ApiManager.cpp`) | Yes (`_server/tests/unit/UsersModuleTest.cpp`, `_server/tests/component/BusinessLogicTest.cpp`, `_client/tests/unit/ApiManagerTest.cpp`) | None |
| Users: users CRUD | Defined | Implemented (`UsersModule`, `UserRepository`, `UsersManagement*`) | Yes (unit + component + integration + client unit) | Add more UI workflow tests |
| Users: roles CRUD | Defined | Implemented (`UsersModule`, `RoleRepository`) | Yes (`UsersModuleTest`, `ApiManagerTest`, `RepositoryIntegrationTest`) | Add more API-level contract tests |
| Module dispatch (non-users) | Defined | `inventory` now dispatches to a real module; `purchase`, `sales`, `management`, `analytics`, and `logs` still return planned/not-implemented errors (`_server/src/BusinessLogic/BusinessLogic.cpp`) | Yes (`_server/tests/component/BusinessLogicTest.cpp`) | Complete remaining modules |
| Inventory: ProductType CRUD | Defined | Implemented across shared endpoint, server module/repository, Qt client wiring, and HTTP transport (`_common/include/common/Endpoints.hpp`, `_server/src/BusinessLogic/InventoryModule.cpp`, `_server/src/Database/ProductTypeRepository.cpp`, `_server/tests/component/HttpContractTest.cpp`, `_client/src/Network/ApiManager.cpp`, `_client/src/Ui/Models/ProductTypesModel.cpp`, `_client/src/Ui/Views/ProductTypesView.cpp`) | Yes (server unit + component + repository integration + HTTP contract + client unit + MainWindow smoke + ProductTypesView Qt workflow tests) | Add live fullstack GUI round-trip coverage and richer error-state/system validation |
| Inventory: stock tracking | Defined | Implemented across shared endpoint, server module/repository, Qt client wiring, and smoke/runtime scripts (`InventoryModule`, `InventoryRepository`, `Stocks*`, `MainWindow`, smoke scripts) | Yes (server unit + component + repository integration + HTTP contract + client unit + MainWindow + StocksView Qt tests + smoke) | Broader inventory workflows and live interactive GUI/system coverage still missing |
| Purchase | Defined | Not implemented end-to-end | No dedicated suite | High |
| Sales | Defined | Not implemented end-to-end | No dedicated suite | High |
| Management | Defined | Not implemented end-to-end | No dedicated suite | High |
| Analytics | Defined | Not implemented | No dedicated suite | High |
| Logs | Defined | Database table plus placeholder route only | No dedicated suite | High |

## Architecture Baseline

- Client: `MainWindow` now lazily hosts users, ProductType, and stock flows through `ApiManager`, `ProductTypes*`, and `Stocks*`, so startup no longer eagerly constructs or fetches those tabs.
- Server: `HttpServer/HttpTransaction` routes into `BusinessLogic`, which now dispatches to both `UsersModule` and `InventoryModule`.
- Data access: `RepositoryManager` now serves `UserRepository`, `RoleRepository`, `ProductTypeRepository`, and `InventoryRepository` over SQLite.
- Runtime configuration: both client and server now accept environment-based host/port settings; the server also accepts DB/bootstrap path overrides, and the client supports an optional auto-exit timer for offscreen smoke runs.

## Quality/Process Baseline

- Jenkins runs checkout, clean, Conan install, `python3 build.py docs`, CMake configure/build, `ctest`, the non-interactive inventory smoke script, the offscreen GUI startup smoke, and archives the built client/server binaries plus the bootstrap SQL and deployment runbook (`Jenkinsfile`).
- VS Code docs workflow renders PlantUML and runs Doxygen via `scripts/vscode/run_task.sh docs`.
- `python3 build.py docs` now checks markdown links, renders PlantUML when Docker is available, and runs Doxygen.
- `python3 build.py smoke` runs `scripts/smoke/fullstack_inventory_smoke.sh` for an isolated server/API smoke path.
- `python3 build.py smoke-gui` runs `scripts/smoke/fullstack_gui_startup_smoke.sh` for a minimal offscreen desktop runtime smoke path.

## Test Coverage Focus (Current)

- Users business logic happy paths and key validation paths.
- ProductType business rules in `InventoryModule`: required fields, duplicate code handling, CRUD dispatch.
- ProductType repository CRUD against a temporary SQLite database.
- ProductType HTTP request/response contract through `HttpTransaction` and real `BusinessLogic`.
- ProductType client API/model behavior in `_client/tests/unit/ApiManagerTest.cpp` and `_client/tests/unit/ProductTypesModelTest.cpp`.
- Product management menu/tab workflow coverage in `_client/tests/unit/MainWindowTest.cpp`.
- ProductTypes view workflow coverage for create/edit/delete/filter interactions in `_client/tests/unit/ProductTypesViewTest.cpp`.
- Stock tracking server rules, repository persistence, HTTP contracts, desktop view workflows, and smoke validation.
- Gap: there is still no full interactive live client/server GUI round-trip test.

## 30/60/90 Plan

### 30 Days

1. Add one repeatable interactive fullstack GUI smoke path for the real Qt client against the real server.
2. Tighten CI docs rendering so PlantUML is enforced instead of opportunistic.
3. Prepare the first honest packaged distribution format beyond archived binaries.

### 60 Days

1. Extend Inventory beyond basic stock CRUD into broader inventory record workflows.
2. Add system-level client/server coverage for Inventory error states and refresh flows.
3. Decide the first honest release bundle format beyond archived binaries.

### 90 Days

1. Deliver second and third non-users slices (recommended: Purchase and Sales base CRUD).
2. Add schema-compatibility coverage for non-users modules.
3. Introduce packaging/publish steps and explicit release gates in CI.

## Acceptance Criteria For Resumed Development

- Every new module must include:
  - Business module + repository implementation.
  - Endpoint wiring and serializer compatibility.
  - At least one server unit test and one integration/contract test.
  - Updated PlantUML and status matrix.
- For the current Inventory baseline specifically, remaining completion gates are:
  - Full live interactive client/server GUI workflow coverage.
  - Release/documentation automation aligned with the actual docs toolchain.
