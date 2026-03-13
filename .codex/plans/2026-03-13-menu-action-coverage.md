## 1) Goal and context

- Ticket/branch: `feature/full-mvp-development`
- Problem statement: `MainWindow` advertises a full menu tree, but most `MainMenuActions::Type` values still fall into an unsupported default path.
- Intended outcome: every menu action opens a concrete tab; `User Roles` becomes a real CRUD view backed by the existing roles API, while still-unimplemented business modules open explicit status/landing views instead of dead ends.

## 2) Scope

- In scope:
  - client-side menu routing completeness
  - reusable landing view for planned modules/settings
  - real roles desktop view using existing `UsersManagement*`
  - client unit/UI tests for new routing behavior
  - status doc update
- Out of scope:
  - inventing new server-side Purchase/Sales/Analytics/Logs backends
  - changing existing HTTP contracts or DB schema
  - packaging/release workflow changes

## 3) Current behavior (baseline)

- What happens today?
  - The menu bar exposes actions for purchasing, sales, analytics, management, logs, and settings.
  - `MainWindow::handleMainMenuAction()` only handles product management, stock tracking, users management, and exit.
  - All other actions log `Unsupported MainMenuAction type.`
- Relevant files/functions:
  - `_client/src/Ui/MainWindow.cpp`
  - `_client/src/Ui/MainWindow.hpp`
  - `_client/src/Ui/MainMenuActions.hpp`
  - `_client/tests/unit/MainWindowTest.cpp`

## 4) Target behavior (DoD)

- [x] Every menu action except `Exit` opens a stable tab instead of hitting an unsupported default path.
- [x] `User Roles` opens a real CRUD view backed by the existing roles API/model/viewmodel.
- [x] Planned modules open explicit status views that truthfully describe current repo support and next slice intent.
- [x] Build succeeds.
- [x] Relevant tests pass.
- [x] No regression in product types, stocks, or users tab behavior.
- [x] Compatibility expectation documented as backward compatible desktop UX expansion.

## 5) Files to touch

| File/Dir | Why |
| --- | --- |
| `_client/src/Ui/MainWindow.cpp` | Complete routing for all menu actions |
| `_client/src/Ui/MainWindow.hpp` | Add lazy creation helpers and tab ownership for new views |
| `_client/src/Ui/Views/ModuleStatusView.*` | Generic landing/status view for not-yet-built modules |
| `_client/src/Ui/Views/RolesView.*` | Real roles CRUD desktop view |
| `_client/tests/include/mocks/ApiManagerMock.hpp` | Stateful role fixtures for UI tests |
| `_client/tests/unit/MainWindowTest.cpp` | Verify menu routing coverage and role fetch behavior |
| `_client/tests/unit/RolesViewTest.cpp` | Verify roles view CRUD workflow |
| `_client/CMakeLists.txt` | Compile new UI sources |
| `_client/tests/unit/CMakeLists.txt` | Compile new test target |
| `docs/Implementation_Status.md` | Record updated desktop/menu baseline truthfully |

## 6) Interfaces and compatibility

- External interfaces impacted:
  - Desktop UI tab routing only.
- Backward compatibility:
  - Backward compatible; existing implemented tabs keep behavior.
- Migration notes:
  - None.

## 7) Approach (implementation steps)

- [x] Add `ModuleStatusView` with reusable high-signal status cards for planned modules and settings.
- [x] Add `RolesView` using `UsersManagementViewModel`, plus shared users-management lazy initialization in `MainWindow`.
- [x] Expand `handleMainMenuAction()` to cover every enum value explicitly and remove the unsupported fallback path.
- [x] Extend tests and mocks for roles CRUD and all-menu routing coverage.
- [x] Update implementation status doc and run validation.

## 8) Risk review

| Risk | Impact | Mitigation/test |
| --- | --- | --- |
| Roles view duplicates or conflicts with shared users model/viewmodel | Broken roles/users refresh behavior | Share one `UsersManagementModel/ViewModel` instance; cover with `MainWindowTest` and `RolesViewTest` |
| Planned-module landing views feel misleading | User confusion about unsupported backend scope | Use explicit wording: planned/not yet implemented, with current repo support bullets |
| Menu coverage regresses later when enum grows | Reintroduction of unsupported paths | Make switch explicit and test representative actions across all menus |
| UI churn breaks current tests | Build/test failures | Run full client/server test suite after edits |

## 9) Verification plan

- `python3 build.py tests --no-clean`
- `ctest --test-dir build --output-on-failure --verbose`
- `python3 build.py docs`
- `git diff --check`

## 10) Progress log

- [16:59] Baseline inspected: unsupported menu path confirmed in `MainWindow`.
- [17:10] Added `ModuleStatusView`, `RolesView`, and full `MainWindow` action routing.
- [17:13] `python3 build.py tests --no-clean` passed; full suite now includes `RolesViewTest`.
- [17:23] `python3 build.py docs` and `git diff --check` passed after status-doc update.

## 11) Final review

- What changed vs plan:
  - Stayed within the planned client-only scope; no server/API/schema expansion was needed.
- What was verified:
  - `python3 build.py tests --no-clean`
  - `ctest --test-dir build --output-on-failure --verbose`
  - `python3 build.py docs`
  - `git diff --check`
- Remaining risks/follow-ups:
  - Planned modules still open truthful landing views rather than real business slices, because those backends do not exist yet.
