## 1) Goal and context

- Ticket/branch: `feature/full-mvp-development`
- Problem statement: The desktop startup flow shows `MainWindow` immediately after opening the login dialog, so the login prompt can be bypassed and the main UI becomes usable before authentication.
- Intended outcome: Gate the desktop UI on successful login and make abandoned login flow exit or return safely instead of leaving a usable or headless app state.

## 2) Scope

- In scope:
  - Client startup/login flow
  - Dialog manager login/registration transitions
  - Focused UI tests for startup gating
- Out of scope:
  - Server authentication behavior
  - Role-based authorization inside feature tabs
  - Registration policy redesign

## 3) Current behavior (baseline)

- What happens today?
  - `main.cpp` creates `MainWindow` before the user is authenticated.
  - Even after the first fix attempt, the top-level window could still exist too early for the user's real runtime expectation.
  - Closing login or registration dialogs can leave the app in a weak state.
- Reproduction steps (if bug):
  - Launch client.
  - Close or ignore the login dialog.
  - Observe that the main UI is already shown.
- Relevant files/functions:
  - `_client/src/main.cpp`
  - `_client/src/Ui/MainWindow.cpp`
  - `_client/src/Ui/Dialogs/DialogManager.cpp`
  - `_client/tests/unit/MainWindowTest.cpp`
  - `_client/tests/unit/DialogManagerTest.cpp`

## 4) Target behavior (DoD)

- [x] Build succeeds
- [x] Relevant tests pass
- [x] No regression in touched flows
- [x] Compatibility expectation documented (backward compatible or explicitly breaking)
- [x] Main window is not shown before successful login
- [x] Successful login reveals the main window
- [x] Closing login before auth quits the app
- [x] Closing registration before auth returns to login instead of leaving the app headless

## 5) Files to touch

| File/Dir | Why |
| --- | --- |
| `_client/src/main.cpp` | Move startup ownership out of preconstructed `MainWindow` |
| `_client/src/Ui/StartupController.*` | Create `MainWindow` only after `loginSucceeded` |
| `_client/src/Ui/MainWindow.hpp` | Remove startup gating from `MainWindow` itself |
| `_client/src/Ui/MainWindow.cpp` | Keep `MainWindow` as the post-authenticated shell only |
| `_client/src/Ui/Dialogs/DialogManager.hpp` | Publish login lifecycle signals |
| `_client/src/Ui/Dialogs/DialogManager.cpp` | Emit login success/cancel events and handle registration close safely |
| `_client/tests/include/mocks/DialogManagerMock.hpp` | Support auth-gated tests |
| `_client/tests/unit/StartupControllerTest.cpp` | Verify startup/login ownership and post-auth window creation |
| `_client/tests/unit/MainWindowTest.cpp` | Keep menu/tab behavior coverage independent of startup |
| `_client/tests/unit/DialogManagerTest.cpp` | Verify dialog-manager login cancel behavior |

## 6) Interfaces and compatibility

- External interfaces impacted (HTTP endpoints, dataset keys, DB schema, config):
  - None
- Backward compatibility:
  - Client startup behavior changes intentionally: authentication becomes mandatory before the main window appears.
- Migration notes (if schema changed):
  - None

## 7) Approach (implementation steps)

- [x] Add `DialogManager` signals for successful and abandoned authentication flow.
- [x] Move startup ownership into a controller that creates `MainWindow` only after login succeeds.
- [x] Quit application on closed login dialog before authentication and restore login when registration is closed.
- [x] Add focused tests for the new startup contract.

Notes:

- Prefer the smallest safe diff.
- Avoid drive-by refactors.

## 8) Risk review

| Risk | Impact | Mitigation/test |
| --- | --- | --- |
| Main window still exists too early | User can see or focus shell before auth | Create the main window only after `loginSucceeded`; verify with dedicated startup test |
| Cancel handling quits too aggressively | Legit flows get interrupted | Only treat login-dialog rejection as quit before auth; registration rejection returns to login |
| Test mocks drift from real dialog lifecycle | False confidence | Keep changes minimal and cover both MainWindow and DialogManager layers |

## 9) Verification plan

List exact commands before execution.

- `cmake --build build --target MainWindowTest StartupControllerTest DialogManagerTest --parallel`
- `ctest --test-dir build --output-on-failure --verbose`
- `python3 build.py smoke-gui`
- `git diff --check`

## 10) Progress log

- [19:35] confirmed startup bug: `MainWindow::startUiProcess()` showed the main window unconditionally after opening login
- [19:41] added dialog-manager auth lifecycle signals and gated `MainWindow` visibility on login success
- [19:45] added login-cancel and focused startup tests; first pass exposed a shared-fixture issue in `DialogManagerTest`
- [19:49] moved dialog-to-dialog wiring into shared `setupDialogConnections()` so wrappers follow production behavior
- [19:50] verified focused client build/tests and whitespace checks passed
- [20:08] switched to a stronger fix: `StartupController` now owns startup and creates `MainWindow` only after `loginSucceeded`
- [20:12] added `StartupControllerTest` and reran the full test suite plus GUI startup smoke

## 11) Final review

- What changed vs plan:
  - Kept the diff client-only.
  - Added a shared dialog-connection setup step so production and test wrappers use the same login/registration lifecycle wiring.
  - Replaced the “hide an already-constructed window” approach with delayed `MainWindow` construction after authentication.
- What was verified:
  - `cmake --build build --target MainWindowTest StartupControllerTest DialogManagerTest --parallel`
  - `ctest --test-dir build --output-on-failure --verbose`
  - `python3 build.py smoke-gui`
  - `git diff --check`
- Remaining risks/follow-ups:
  - This fix covers startup gating and abandoned login handling only; there is still no logout-driven re-authentication flow in the client.
