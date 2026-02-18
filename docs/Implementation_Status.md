# SageStore Implementation Status

## Snapshot Date

- 2026-02-18

## Scope Matrix

| Area | Documentation Target | Implemented in Code | Tests | Gap |
| --- | --- | --- | --- | --- |
| Users: login | Defined | Implemented (`_server/src/BusinessLogic/UsersModule.cpp`, `_client/src/Network/ApiManager.cpp`) | Yes (`_server/tests/unit/UsersModuleTest.cpp`, `_server/tests/component/BusinessLogicTest.cpp`, `_client/tests/unit/ApiManagerTest.cpp`) | None |
| Users: users CRUD | Defined | Implemented (`UsersModule`, `UserRepository`, `UsersManagement*`) | Yes (unit + component + integration + client unit) | Add more UI workflow tests |
| Users: roles CRUD | Defined | Implemented (`UsersModule`, `RoleRepository`) | Yes (`UsersModuleTest`, `ApiManagerTest`, `RepositoryIntegrationTest`) | Add more API-level contract tests |
| Module dispatch (non-users) | Defined | Placeholder modules return explicit `not implemented` errors (`BusinessLogic`) | Yes (`BusinessLogicTest`) | Replace placeholders with real domain logic |
| Purchase | Defined | Not implemented end-to-end | No dedicated suite | High |
| Sales | Defined | Not implemented end-to-end | No dedicated suite | High |
| Inventory | Defined | Not implemented end-to-end | No dedicated suite | High |
| Management | Defined | Not implemented end-to-end | No dedicated suite | High |
| Analytics | Defined | Not implemented | No dedicated suite | High |

## Architecture Baseline

- Client: `MainWindow` + `DialogManager` + Users MVVM + `ApiManager` + `NetworkService`.
- Server: `HttpServer/HttpTransaction` -> `BusinessLogic` module dispatch -> `UsersModule` -> repositories.
- Data access: Repository pattern over SQLite manager.

## Quality/Process Baseline

- CI build and test pipeline exists in `Jenkinsfile`.
- Branch hardening adds stricter server validation and error handling.
- Doxygen config is now repository-portable (`Doxyfile`).
- Markdown link checks are automated via `scripts/check_docs_links.py` and CI.
- VS Code tasks provide reproducible build/test/doc workflows (`.vscode/tasks.json`).

## Test Coverage Focus (Current)

- Users business logic happy paths and key validation paths.
- BusinessLogic module dispatch error handling.
- Client API signal behavior and model reactions.
- Repository CRUD against real SQLite DB for users/roles.

## 30/60/90 Plan

### 30 Days

1. Stabilize users module quality gates.
2. Add integration tests for additional repositories beyond users/roles.
3. Enforce CI checks for docs generation and static style checks.

### 60 Days

1. Deliver one non-users vertical slice end-to-end (recommended: Inventory ProductType).
2. Add API contract tests for the new module.
3. Expand client MVVM test coverage for table interactions and error flows.

### 90 Days

1. Deliver second and third vertical slices (recommended: Purchase and Sales base CRUD).
2. Add migration strategy and schema compatibility tests.
3. Introduce release checklist with feature-completion and test-readiness gates.

## Acceptance Criteria For Resumed Development

- Every new module must include:
  - Business module + repository implementation.
  - Endpoint wiring and serializer compatibility.
  - At least one server unit test and one integration/contract test.
  - Updated PlantUML and status matrix.
