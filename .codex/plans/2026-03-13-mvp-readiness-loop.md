## 1) Goal and context

- Ticket/branch: `feature/full-mvp-development`
- Problem statement: the repo now has an initial inventory baseline, but the MVP still carries avoidable client runtime/threading risk, stale desktop/readiness docs, weak stock integrity guarantees, and an incomplete deployment-validation story.
- Intended outcome: improve practical MVP readiness without inventing unsupported modules by reducing GUI startup cost/risk, hardening stock contracts and persistence integrity, and adding honest deployment/smoke-validation paths grounded in the current binaries and HTTP API.

## 2) Scope

- In scope:
  - lazy-create heavy client tabs/models/views instead of constructing them all at app startup
  - remove duplicate ProductType/users fetches caused by current construction flow
  - add ProductType search/filter behavior and improve basic view responsiveness/polish
  - add view-level Qt tests for ProductType CRUD triggers/filtering and stronger MainWindow startup assertions
  - add stock desktop view/runtime coverage and reopen-refresh behavior
  - harden stock validation, persistence integrity, and missing-resource semantics
  - make server/client endpoint configuration overridable for local deployment/smoke runs
  - add non-interactive server/API and GUI startup smoke scripts
  - improve archived deployment artifacts for first-run bootstrap reality
  - wire smoke/deploy steps into docs/build/CI only where safe and evidence-based
  - update release-readiness/docs/status artifacts
- Out of scope:
  - full Purchase/Sales/Management implementations
  - installer packaging (NSIS/Inno Setup)
  - deep Qt model/view rewrite away from existing `QTableWidget`
  - auth/session redesign

## 3) Current behavior (baseline)

- `MainWindow` eagerly constructs ProductType and Users MVVM/UI objects during startup, causing fetches before the user opens those tabs.
- `UsersManagementModel` fetches users/roles in its constructor, while `UsersView` also fetches users in its constructor.
- `ProductTypesModel` fetches in its constructor, while `ProductTypesView` also fetches in its constructor.
- ProductType UI has CRUD wiring, but no search/filter, no view-level CRUD automation, and only minimal styling.
- Server and client runtime endpoints are hardcoded in `_server/src/main.cpp` and `_client/src/main.cpp`.
- There is no non-interactive smoke script that starts the server, exercises real HTTP CRUD, and exits cleanly.
- Stock integrity relies too heavily on application logic and does not reject missing resources cleanly enough.

## 4) Target behavior (DoD)

- [ ] Main window startup does not eagerly create/fetch ProductType or Users tabs until opened
- [ ] ProductType and Users tabs fetch at most once on first creation, not twice
- [ ] ProductType UI supports search/filter over the loaded list
- [ ] ProductType UI updates table/status efficiently and has modest visual polish without a redesign
- [ ] ProductType view-level Qt tests cover create/edit/delete trigger paths and filtering
- [ ] MainWindow tests prove lazy loading behavior
- [ ] Server/client endpoint settings can be overridden for smoke/deploy runs without recompiling
- [ ] A non-interactive smoke script validates real ProductType CRUD against a running server
- [ ] CI/build/docs/release docs reflect the new smoke/deploy reality truthfully
- [ ] Build/tests/docs checks pass for touched areas

## 5) Files to touch

| File/Dir | Why |
| --- | --- |
| `.codex/plans/2026-03-13-mvp-readiness-loop.md` | required ExecPlan |
| `_client/src/Ui/MainWindow.*` | lazy-create tabs and avoid startup lag |
| `_client/src/Ui/Views/BaseView.*` | shared view responsiveness/polish hooks |
| `_client/src/Ui/Views/ProductTypesView.*` | filter UI, efficient table refresh, test seam |
| `_client/src/Ui/ViewModels/ProductTypesViewModel.*` | filtered list behavior |
| `_client/src/Ui/Views/UsersView.cpp` | remove duplicate fetch |
| `_client/src/main.cpp` | env-configurable endpoint settings |
| `_server/src/main.cpp` | env-configurable bind/db settings |
| `_client/tests/include/mocks/ApiManagerMock.hpp` | counters/spies for lazy-load assertions |
| `_client/tests/unit/MainWindowTest.cpp` | startup + lazy-load test coverage |
| `_client/tests/unit/ProductTypesViewTest.cpp` | view-level CRUD/filter tests |
| `_client/tests/unit/CMakeLists.txt` | compile new Qt test |
| `build.py` | safer canonical docs path and/or smoke helper entry point if warranted |
| `Jenkinsfile` | add smoke validation and/or artifact archiving only where safe |
| `scripts/` | new non-interactive fullstack smoke helper |
| `README.md` and `docs/*.md` | deployment runbook, status, release-readiness updates |

## 6) Interfaces and compatibility

- External interfaces impacted:
  - client/server runtime configuration via environment variables
  - optional smoke script entrypoints
  - Qt UI behavior for lazy tab creation and ProductType filtering
  - stock API validation/error behavior for missing IDs and invalid employee references
- Backward compatibility:
  - default endpoint host/port/path behavior must remain unchanged when env vars are absent
  - ProductType HTTP contract remains unchanged
- Migration notes:
  - new database bootstrap schema now enforces `UNIQUE(productTypeId)` for `Inventory`
  - existing databases created before this change need rebuild/manual migration to gain that DB-level invariant
  - smoke script should prefer an isolated temp DB path when using env overrides

## 7) Approach (implementation steps)

- [ ] Step 1: add runtime configuration overrides for server/client entrypoints
- [ ] Step 2: refactor `MainWindow` to lazy-create Users/ProductType tab stacks and remove duplicate fetches
- [ ] Step 3: add ProductType search/filter and view responsiveness/polish improvements
- [ ] Step 4: add Qt view/startup tests for lazy loading and ProductType CRUD/filter behavior
- [ ] Step 5: harden stock validation, missing-resource behavior, and DB integrity
- [ ] Step 6: add server/API and GUI startup smoke scripts
- [ ] Step 7: update CI/build/docs/release-readiness artifacts and run validation

## 8) Risk review

| Risk | Impact | Mitigation/test |
| --- | --- | --- |
| Lazy MVVM creation breaks existing menu behavior | tabs stop opening or fetch incorrectly | extend `MainWindowTest` with startup/open/reopen assertions |
| Filter logic drifts from displayed data | confusing UX or stale table state | view-level Qt test with seeded dataset |
| Runtime env overrides break default startup | local/developer workflow regression | preserve defaults and add narrow parsing helpers |
| Smoke script flakes on startup timing | false CI failures | add bounded wait/retry and explicit cleanup |
| DB-level stock uniqueness breaks old test assumptions | failing tests or legacy DB divergence | update tests to use dedicated product types and document migration caveat |
| Mocked dialog lifecycle hides raw-pointer bugs | test-only crashes mask real ownership issues | initialize dialog pointers defensively and rerun Qt tests |
| CI changes overreach | broken pipeline | keep Jenkins changes additive and grounded in existing commands |

## 9) Verification plan

- `python3 build.py tests --no-clean`
- `ctest --test-dir build --output-on-failure --verbose`
- `python3 build.py docs`
- `python3 build.py smoke`
- `python3 build.py smoke-gui`
- `git diff --check`

## 10) Progress log

- [done] lazy tab creation and duplicate-fetch removal for users/product types/stocks
- [done] ProductType filter/view tests and stock desktop view wiring/tests
- [done] runtime env overrides for client/server plus client auto-exit for smoke
- [done] stock contract hardening for missing IDs, positive employee IDs, FK enforcement, and DB-level uniqueness on fresh schemas
- [done] inventory API smoke and offscreen GUI startup smoke scripts wired into `build.py`, Jenkins, docs, and release workflow diagram
- [done] deployment/readiness docs reconciled with the actual stock desktop scope and artifact reality

## 11) Final review

- What changed vs plan:
  - scope expanded from ProductType-only readiness to the broader inventory baseline because the worktree already contained stock desktop/client/server additions
  - added stock integrity hardening and GUI startup smoke after review found real deployment/contract gaps
- What was verified:
  - canonical build/tests, full ctest suite, inventory smoke, GUI startup smoke, docs build/render, and diff hygiene
- Remaining risks/follow-ups:
  - no interactive end-to-end GUI workflow automation against a live server yet
  - archived artifacts are still not a self-contained installer/bundle
  - Purchase/Sales/Management/Logs/Analytics remain incomplete
