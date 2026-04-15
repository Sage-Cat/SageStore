# Inventory/ProductType Release Readiness

## Snapshot

- Date: 2026-03-31
- Verdict: smoke-validated development slice, but not release-ready as a complete product shipment

## Evidence Matrix

| Area | Repo evidence | Status | Release impact |
| --- | --- | --- | --- |
| Server implementation | `InventoryModule`, `ProductTypeRepository`, shared endpoint constant, and SQLite schema are present | Ready | ProductType CRUD exists on the server side |
| Client implementation | `ApiManager`, `ProductTypesModel`, `ProductTypesViewModel`, `ProductTypesView`, and `MainWindow` tab wiring are present | Ready as a minimal baseline | User flow exists in code, with search/filter and view-level CRUD automation, but it is still a thin UI |
| Automated tests | ProductType server unit/component/repository tests, HTTP contract tests, client `ApiManager`/`ProductTypesModel` unit tests, `MainWindow` Product Management tab coverage, `ProductTypesView` workflow tests, and a live client/server `FullstackInventoryWorkflowTest` exist | Strong baseline | Good confidence for the implemented slice, including a real interactive workflow against the server |
| CI pipeline | `Jenkinsfile` runs checkout, clean, Conan install, `python3 build.py docs`, CMake configure/build, `ctest`, inventory smoke, GUI startup smoke, and artifact archiving | Stronger baseline | Build/test/smoke automation now exists for the current runtime shape |
| Docs build path | `scripts/vscode/run_task.sh docs` renders PlantUML and runs Doxygen; `python3 build.py docs` now runs link check, optional PlantUML render, and Doxygen; Jenkins uses `python3 build.py docs` | Partial | The path is more unified, but PlantUML render still depends on Docker availability |
| Packaging/deployment | Jenkins archives the built binaries, bootstrap SQL, and deployment runbook, but no installer or publish stage exists | Partial | Release remains manual, first-run assets now exist, and the binaries still depend on external shared libraries |

## Current Readiness Reading

The repo now has a coherent inventory baseline that can be exercised from the shipped desktop client, through HTTP contract tests, through a non-interactive inventory smoke script against the real server binary, through an offscreen GUI startup smoke, and through a live Qt client/server inventory CRUD workflow. It is still not honest to call the repository release-ready because PlantUML rendering is still conditional in CI, Jenkins does not produce a self-contained installer or publishable release bundle, and the wider ERP foundations around stock ownership, movement semantics, packaging, and migration discipline are still incomplete.

## Minimum Gates Before Calling This Slice Release-Ready

1. Keep the live Qt client/server inventory workflow green in CI and add installed-binary validation around it.
2. Enforce PlantUML rendering in CI, not only when Docker is available.
3. Define how a release is packaged or distributed beyond archived binaries.
4. Add migration/backup/readiness basics for any user-facing release claim, including `/api/system/health`.

## Supporting Commands Already Present

- `python3 build.py all`
- `python3 build.py tests`
- `python3 build.py test`
- `python3 build.py smoke`
- `python3 build.py smoke-gui`
- `scripts/smoke/fullstack_inventory_smoke.sh`
- `scripts/smoke/fullstack_gui_startup_smoke.sh`
- `python3 scripts/check_docs_links.py`
- `scripts/vscode/run_task.sh docs`
