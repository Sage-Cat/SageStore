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
| Inventory: ProductType CRUD | Defined | Implemented across shared endpoint, server module/repository, Qt client wiring, and HTTP transport (`_common/include/common/Endpoints.hpp`, `_server/src/BusinessLogic/InventoryModule.cpp`, `_server/src/Database/ProductTypeRepository.cpp`, `_server/tests/component/HttpContractTest.cpp`, `_client/src/Network/ApiManager.cpp`, `_client/src/Ui/Models/ProductTypesModel.cpp`, `_client/src/Ui/Views/ProductTypesView.cpp`) | Yes (server unit + component + repository integration + HTTP contract + client unit/UI smoke) | Add view-level CRUD/system tests and search/filter behavior |
| Inventory: stock tracking | Defined | Not implemented end-to-end | No dedicated suite | High |
| Purchase | Defined | Not implemented end-to-end | No dedicated suite | High |
| Sales | Defined | Not implemented end-to-end | No dedicated suite | High |
| Management | Defined | Not implemented end-to-end | No dedicated suite | High |
| Analytics | Defined | Not implemented | No dedicated suite | High |
| Logs | Defined | Database table plus placeholder route only | No dedicated suite | High |

## Architecture Baseline

- Client: `MainWindow` now hosts both users and ProductType flows through `ApiManager`, `ProductTypesModel`, `ProductTypesViewModel`, and `ProductTypesView`.
- Server: `HttpServer/HttpTransaction` routes into `BusinessLogic`, which now dispatches to both `UsersModule` and `InventoryModule`.
- Data access: `RepositoryManager` now serves `UserRepository`, `RoleRepository`, and `ProductTypeRepository` over SQLite.

## Quality/Process Baseline

- Jenkins runs checkout, clean, Conan install, markdown link check, CMake configure/build, and `ctest` (`Jenkinsfile`).
- VS Code docs workflow renders PlantUML and runs Doxygen via `scripts/vscode/run_task.sh docs`.
- `python3 build.py docs` checks markdown links and runs Doxygen, but does not render PlantUML images.

## Test Coverage Focus (Current)

- Users business logic happy paths and key validation paths.
- ProductType business rules in `InventoryModule`: required fields, duplicate code handling, CRUD dispatch.
- ProductType repository CRUD against a temporary SQLite database.
- ProductType HTTP request/response contract through `HttpTransaction` and real `BusinessLogic`.
- ProductType client API/model behavior in `_client/tests/unit/ApiManagerTest.cpp` and `_client/tests/unit/ProductTypesModelTest.cpp`.
- Product management menu/tab workflow coverage in `_client/tests/unit/MainWindowTest.cpp`.
- Gap: there is still no view-level CRUD automation or full live client/server GUI round-trip test for ProductType.

## 30/60/90 Plan

### 30 Days

1. Add Qt workflow coverage for `ProductTypesView` create/edit/delete flows and error handling.
2. Add one repeatable fullstack GUI smoke path for ProductType CRUD.
3. Make PlantUML rendering part of CI, or move CI to the existing docs task entrypoint.

### 60 Days

1. Add search/filter behavior promised in the product docs for Product Types.
2. Extend Inventory beyond ProductType into actual stock/inventory record workflows.
3. Add system-level client/server coverage for Inventory error states and refresh flows.

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
- For the Inventory/ProductType slice specifically, remaining completion gates are:
  - GUI/system workflow coverage beyond menu/tab smoke and model/unit tests.
  - Release/documentation automation aligned with the actual docs toolchain.
