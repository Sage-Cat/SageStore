# Inventory/ProductType Release Readiness

## Snapshot

- Date: 2026-03-13
- Verdict: development-ready slice, but not release-ready as a complete product shipment

## Evidence Matrix

| Area | Repo evidence | Status | Release impact |
| --- | --- | --- | --- |
| Server implementation | `InventoryModule`, `ProductTypeRepository`, shared endpoint constant, and SQLite schema are present | Ready | ProductType CRUD exists on the server side |
| Client implementation | `ApiManager`, `ProductTypesModel`, `ProductTypesViewModel`, `ProductTypesView`, and `MainWindow` tab wiring are present | Ready as a minimal baseline | User flow exists in code, but it is still a thin UI |
| Automated tests | ProductType server unit/component/repository tests, HTTP contract tests, client `ApiManager`/`ProductTypesModel` unit tests, and `MainWindow` Product Management tab coverage exist | Strong baseline | Good confidence for the implemented slice |
| CI pipeline | `Jenkinsfile` runs checkout, clean, Conan install, docs link check, CMake configure/build, and `ctest` | Partial | Build/test automation exists, but only for part of the release story |
| Docs build path | `scripts/vscode/run_task.sh docs` renders PlantUML and runs Doxygen; `python3 build.py docs` runs link check + Doxygen only; Jenkins only runs docs link check | Partial | Documentation outputs are not validated the same way in all entrypoints |
| Packaging/deployment | No packaging, artifact publish, or deploy stage exists in `Jenkinsfile` | Not ready | Release remains manual and underspecified |

## Current Readiness Reading

The repo now has a coherent first non-users slice that can be exercised from the shipped desktop client and through HTTP contract tests. It is still not honest to call the repository release-ready because there is no live fullstack GUI proof that the client works against the server, CI does not validate the full docs pipeline, and Jenkins does not produce or publish release artifacts.

## Minimum Gates Before Calling This Slice Release-Ready

1. Add GUI/system validation for the ProductTypes view create/edit/delete flows against the real view.
2. Add a live client/server smoke test or scripted fullstack verification path.
3. Pick one docs build path and enforce it in CI, including PlantUML rendering.
4. Define how a release is packaged or deployed, since current Jenkins stops at build/test.
5. Resolve the self-registration policy mismatch before any user-facing release claim.

## Supporting Commands Already Present

- `python3 build.py all`
- `python3 build.py tests`
- `python3 build.py test`
- `python3 scripts/check_docs_links.py`
- `scripts/vscode/run_task.sh docs`
