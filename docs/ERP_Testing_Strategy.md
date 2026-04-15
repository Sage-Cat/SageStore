# SageStore ERP Testing Strategy

## Purpose

This document defines the test architecture required to move SageStore from a
credible ERP MVP to a safely installable store system.

The primary decision is deliberate:

- use the existing `GTest` and `Qt Test` stack as the main automation framework
- keep HTTP contract tests and SQLite-backed integration tests as the server truth layer
- add Qt-driven client/server system tests for real workflows instead of introducing a new UI framework first

That keeps the test system aligned with the current codebase and avoids an unnecessary tool jump.

## Core principles

- Put invariants in server-side tests first, then prove operator workflows through UI/system tests.
- Keep the fastest tests closest to the code, and reserve slower fullstack tests for the highest-value workflows.
- Test the generic-store core separately from optional vertical extensions such as autoparts compatibility.
- Treat release/install validation as part of the test architecture, not as a postscript.

## Recommended test stack

| Layer | Scope | Current state | Required direction | Main tool |
| --- | --- | --- | --- | --- |
| Unit | parsing, validation, status normalization, view-model rules | strong | keep expanding around new domain services | `GTest`, `Qt Test` |
| Repository integration | SQLite schema + repository behavior | strong baseline | add new entities and migration checks | `GTest` + temp SQLite |
| HTTP contract | endpoint serialization and workflow-side errors | strong baseline | keep every new route additive and covered | `GTest` + `HttpTransaction` |
| Client widget/unit | local MVVM behavior, dialogs, action enablement | strong baseline | keep for every desktop workspace | `Qt Test` |
| Client/server system | real widgets against real server binary and real DB bootstrap | inventory plus key management/purchase/sales slices now covered | expand to auth failures, CRUD-heavy flows, and posting/cancel lifecycles | `Qt Test` + `QProcess` |
| Smoke/release | built binaries, startup/runtime, docs, links | baseline exists | add packaging/install and upgrade checks | shell scripts + `build.py` |

## UI automation choice

- Primary choice: `Qt Test` with widget `objectName` hooks and in-process UI control.
- Reason:
  - the repo already uses `QTest::mouseClick`, `QSignalSpy`, `QTRY_*`, and stable widget names
  - it can drive modal dialogs directly from code
  - it keeps test code in the same language and build system as the product
- Current implementation seam:
  - [FullstackServerHarness](../_client/tests/include/wrappers/FullstackServerHarness.hpp) starts a real server process and waits on a neutral `/api/system/health` probe
  - [FullstackInventoryWorkflowTest](../_client/tests/system/FullstackInventoryWorkflowTest.cpp) drives real login plus inventory CRUD
  - [FullstackMainWindowWorkflowTest](../_client/tests/component/FullstackMainWindowWorkflowTest.cpp) smoke-loads the major workspaces against live data
  - [FullstackWorkflowTest](../_client/tests/integration/FullstackWorkflowTest.cpp) exercises management contact creation, purchase receipt posting, and sales invoice preview through the real server
  - [FullstackSalesInvoiceExportTest](../_client/tests/component/FullstackSalesInvoiceExportTest.cpp) verifies invoice export file contents against live backend data
- Do not add a separate external UI framework yet.
- Consider an external black-box UI tool only later if installed-package or Windows-native flows must be exercised in ways that in-process Qt tests cannot reach.

## Workflow matrix

| Workflow | Unit/repository focus | HTTP contract focus | UI/system focus | Release gate |
| --- | --- | --- | --- | --- |
| Login and user/role admin | password/login validation, role mutations | login/users/roles routes | login dialog, user CRUD, role CRUD | startup + auth smoke |
| Management master data | contacts/suppliers/employees/company rules | management routes | contacts/suppliers/employees/company workflows | seeded demo/build smoke |
| Product catalog and stock | product validation, stock ledger rules, catalog attributes | inventory routes | product CRUD, stock adjustments, search/filter | inventory smoke + system |
| Supplier catalog import | parser, preview, validation, dedupe | staged import endpoints/jobs | import preview and commit workflow | file-format fixture smoke |
| Purchase and receipt posting | lifecycle rules, receipt side effects, attachment staging | purchase/orders/records/receipts/invoices | order creation, line editing, receipt posting, invoice registration | end-to-end receive flow |
| Sales fulfillment | reservation/posting lifecycle, export renderers | sales/orders/records/exports | sales creation, posting, export preview/output | outbound document smoke |
| Logs and analytics | query rules, aggregations | analytics/log routes | dashboard refresh and filter flows | support/ops report gate |
| Packaging/install | migration, config defaults, backup/restore | health/readiness if exposed | installed binary startup and first-run workflow | installer/upgrade gate |

## Current important-feature coverage map

| Feature area | Strongest current backend coverage | Strongest current desktop/offscreen coverage | Offscreen regression gate | Main remaining gap |
| --- | --- | --- | --- | --- |
| Auth and startup | `UsersModuleTest`, `BusinessLogicTest`, `System` health path | `DialogManagerTest`, `StartupControllerTest`, `System_DebugCliScenarioTest` | `System_DebugCliScenarioTest` login + workspace scenarios | live auth failure/retry/registration matrix is still thin |
| Users and roles | `UsersModuleTest`, `RepositoryIntegrationTest`, `HttpContractTest` | `UsersViewTest`, `RolesViewTest`, `ErpModelsTest`, workspace-load coverage | `offscreen_regression` CTest lane + Debug CLI workspace capture | no live CRUD-heavy GUI/system scenario yet |
| Management master data | `BusinessLogicTest`, `RepositoryIntegrationTest`, `HttpContractTest` | `ManagementViewTest`, `ErpModelsTest`, `FullstackWorkflowTest`, workspace-load coverage | `offscreen_regression` CTest lane + management workspace capture | supplier/employee live write flows are still sparse |
| Inventory and supplier catalog | `InventoryModuleTest`, `RepositoryIntegrationTest`, `HttpContractTest` | `ProductTypes*`, `Stocks*`, `SupplierCatalogViewTest`, `FullstackInventoryWorkflowTest` | `offscreen_regression` CTest lane + inventory/stock captures + inventory smoke | no storage/company/ledger depth yet |
| Purchase and receipts | `ErpModulesTest`, `HttpContractTest`, repository integration | `PurchaseViewTest`, `ErpModelsTest`, `FullstackWorkflowTest` | `offscreen_regression` CTest lane + purchase workspace capture | order edit/delete and invoice attachment flows remain open |
| Sales and invoicing | `HttpContractTest` plus partial module checks | `SalesViewTest`, `ErpModelsTest`, `FullstackWorkflowTest`, `FullstackSalesInvoiceExportTest` | `offscreen_regression` CTest lane + sales/invoicing captures | backend sales lifecycle and cancel/posting depth is still weak |
| Analytics and logs | `BusinessLogicTest`, `HttpContractTest`, `ErpModulesTest` | `AnalyticsViewTest`, `ErpModelsTest`, workspace-load coverage | `offscreen_regression` CTest lane + analytics capture | richer filtering and drill-down remain future work |
| Runtime/operability | smoke scripts and `system/health` | offscreen client startup smoke, Debug CLI artifact capture | `python3 build.py regression` | installed package/upgrade validation remains open |

## Immediate implementation roadmap for tests

### Phase 1

- Keep all current unit/integration/contract tests green.
- Keep the current live Qt workflow suite green:
  - login + product types
  - management contact creation
  - purchase order receive
  - sales invoice preview/export
  - broad workspace loading
- Expand next into:
  - login failure and registration
  - users/roles CRUD
  - supplier and employee CRUD
  - purchase-order create/edit/delete and line-item editing
  - sales create/edit/post/cancel flows
- Add export cancel/failure-path coverage alongside the existing file-content verification.

### Phase 2

- Add company/storage-aware repository integration tests.
- Add stock-ledger contract tests for receipts, adjustments, transfers, and sales posting.
- Add cross-company visibility and query tests.

### Phase 3

- Add invoice-ingestion pipeline tests:
  - parser fixtures
  - preview validation summaries
  - idempotent commit behavior
  - attachment storage and retrieval
  - background-job retries and status reporting

### Phase 4

- Add sales fulfillment system tests:
  - reserve stock
  - post sale
  - cancel/reverse
  - render HTML/PDF outputs
  - audit export actions

### Phase 5

- Add installer/upgrade/backup-restore checks to the release pipeline.
- Add longer-running nightly suites for broad workflow and seeded-dataset regression.

## Coverage expectations by module

| Module | Minimum bar for a new feature |
| --- | --- |
| `_server` business module | unit test for invariants + HTTP contract/integration test |
| `_server` repository/schema | repository integration test + migration test if schema changes |
| `_client` view model/view | Qt unit/widget test |
| End-to-end operator workflow | Qt client/server system test if the UI is operator-facing |
| Docs/build/release scripts | smoke or command-level validation plus docs link check |

## Offscreen regression gate

- Primary background gate: `python3 build.py regression`
- Direct script form: `bash scripts/smoke/offscreen_feature_regression.sh`
- The gate keeps the existing Qt/GTest/Debug CLI stack and runs in WSL/headless mode with:
  - `QT_QPA_PLATFORM=offscreen`
  - `SAGESTORE_FORCE_OFFSCREEN=1`
  - isolated `SAGESTORE_LOG_DIR` output
- It must fail when any of the following happen:
  - `offscreen_regression` labeled CTest cases fail
  - inventory or GUI smoke fails
  - runtime logs contain `ERROR` or `CRITICAL`
  - Debug CLI screenshots/state dumps are missing, invalid, or obviously blank
- Current Debug CLI scenario corpus checks:
  - login path
  - inventory/product management workspace rendering
  - stock tracking rendering
  - management workspace rendering
  - purchase workspace rendering
  - sales order and invoicing rendering
  - users and roles rendering
  - analytics rendering
  - failure-artifact capture on selector errors

## Commands

Use these as the default verification stack:

```bash
python3 build.py tests
python3 build.py test
ctest --test-dir build --output-on-failure --verbose -R System_FullstackInventoryWorkflowTest
ctest --test-dir build --output-on-failure --verbose -R Fullstack
python3 build.py smoke
python3 build.py smoke-gui
python3 build.py regression
python3 scripts/check_docs_links.py
python3 build.py docs
```
