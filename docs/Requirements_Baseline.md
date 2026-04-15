# SageStore Requirements Baseline

## Snapshot

- Date: 2026-03-25
- Current-state source of truth: `docs/Implementation_Status.md` plus the codebase and tests
- Target-state source of truth: `docs/Project_Documentation.md`
- Future-direction source of truth: `docs/Future_Architecture_and_Design_Roadmap.md`
- Supporting evidence: `README.md`, `Jenkinsfile`, `scripts/create_db.sql`, existing PlantUML assets, build/test scripts, and the actual module wiring under `_client/`, `_server/`, and `_common/`

## Reconciliation Summary

- The target product documentation still describes a broader ERP than the repository currently implements.
- The future-state roadmap should stay separate from current-state truth so the repo can communicate both honestly.
- Current end-to-end reality now includes:
  - users login
  - users CRUD
  - roles CRUD
  - settings with persisted server defaults and language selection
  - English and Ukrainian desktop UI support
  - inventory ProductType CRUD
  - stock tracking
  - supplier catalog mappings with desktop CSV import
  - management CRUD for contacts, suppliers, and employees
  - purchase orders, order lines, and goods receipt posting into stock
  - sales orders, order lines, and simple desktop invoice export
  - audit-log browsing plus mutation audit writes
  - read-only sales and inventory analytics summaries
- Still outside current evidence-backed implementation:
  - company management
  - incoming invoice registration/upload
  - barcode/label PDF generation and printing
  - advanced invoice export formats such as HTML/PDF/XML
  - dedicated self-profile editing
  - mature packaging/publish automation

## Actors

| Actor | Type | Evidence | Current effective scope |
| --- | --- | --- | --- |
| Administrator | Business | `docs/Project_Documentation.md` user stories | Login, manage users, manage roles, use the rest of the desktop ERP baseline |
| User | Business | `docs/Project_Documentation.md` user stories | Login, manage inventory, master data, purchase, sales, logs, and analytics in the current baseline |
| Desktop client | Technical consumer | `_client/` implementation | Full consumer for the implemented ERP slices |
| Future API consumer | Technical consumer | repo architecture constraints and current HTTP server design | Must be supported by stable, resource-oriented, consumer-agnostic HTTP contracts |
| Developer / CI | Delivery actor | `Jenkinsfile`, `build.py`, `scripts/` | Builds, tests, docs checks, smoke validation, release-readiness evidence |

## Use-Case Catalog

| Use case | Primary actor | Target doc status | Current repo status | Evidence |
| --- | --- | --- | --- | --- |
| Login | Administrator, User | Defined | Implemented | `UsersModule`, `ApiManager`, dialogs, tests |
| Create/edit/delete users | Administrator | Defined | Implemented | `UsersModule`, `UserRepository`, `UsersManagement*`, tests |
| Assign roles | Administrator | Defined | Implemented | `UsersModule`, `RoleRepository`, tests |
| Edit own profile | User | Defined | Not implemented as a separate flow | no dedicated endpoint/UI flow |
| Configure desktop settings and language | Administrator, User | Not explicit in target doc, required by current branch goal | Implemented | `AppSettings`, `SettingsView`, `main.cpp`, `TsTranslator`, tests |
| Manage Product Types | User | Defined | Implemented baseline | `InventoryModule`, `ProductTypeRepository`, `ProductTypes*`, tests |
| View stock / inventory records | User | Defined | Implemented baseline | `InventoryModule`, `InventoryRepository`, `Stocks*`, tests, smoke |
| Manage supplier product mappings | User | Defined | Implemented baseline | `SuppliersProductInfoRepository`, `InventoryModule`, `SupplierCatalog*`, tests |
| Import supplier catalog from CSV | User | Defined | Implemented as desktop CSV import | `SupplierCatalogView.cpp` |
| Manage employees, suppliers, customers/contacts | User | Defined | Implemented baseline | `ManagementModule`, repositories, `ManagementView`, tests |
| Manage companies | User | Defined | Not implemented | no shared entity, repository, or UI slice |
| Manage purchase orders and order records | User | Defined | Implemented baseline | `PurchaseModule`, repositories, `PurchaseView`, tests |
| Post goods receipts into stock | User | Defined | Implemented baseline | `/api/purchase/receipts`, tests |
| Register or upload incoming invoices | User | Defined | Not implemented | no file/attachment workflow |
| Manage sales invoices/orders | User | Defined | Implemented baseline | `SalesModule`, repositories, `SalesView`, tests |
| Export invoices | User | Defined | Implemented as simple text export | `SalesView.cpp` |
| Export invoices to HTML/PDF/XML | User | Defined | Not implemented | no renderer/export adapters |
| View logs / history | User | Defined | Implemented baseline | `LogsModule`, audit writes, `LogsView`, tests |
| Run analytics / reports | User | Defined | Implemented baseline | `AnalyticsModule`, `AnalyticsView`, tests |
| Generate barcode PDFs / print labels | User | Defined | Not implemented | no label-generation pipeline |

## Assumptions Register

| ID | Assumption | Reasoning / evidence | Impact |
| --- | --- | --- | --- |
| A1 | The server API must stay consumer-agnostic and resource-oriented | repo instructions explicitly require modern API design beyond the desktop client | endpoint contracts remain additive and not Qt-specific |
| A2 | `Contact` is the canonical shared data model for customer/client-style parties | schema has `Contact`, while docs alternate between contact/client/customer | management and sales use `Contact` as the storage entity |
| A3 | `Sales` is the canonical module name for outward API/docs, while `SaleOrder` remains the current schema/entity name | docs and routes use `sales`; code/schema use `SaleOrder` | outward docs normalize to `Sales` without forced internal rename |
| A4 | Self-registration is not a release requirement | target docs say user creation is admin-driven, while the registration dialog still exists | registration remains a legacy bootstrap/testing behavior until policy is resolved |
| A5 | Language changes are safely applied on restart for now | startup translation loading is implemented; live hot-switch would require wider widget refresh handling | settings truthfully instruct the user to restart after language change |
| A6 | Supplier catalog import currently means desktop CSV ingestion into `SuppliersProductInfo` mappings | repo now implements CSV import in the client but not XML/XLSX or invoice-driven creation | docs must distinguish implemented CSV import from broader target import scope |
| A7 | Sales baseline currently covers order/invoice management and export, but not automatic inventory decrement | `SalesModule` does not touch stock repositories today | inventory effects of sales remain a documented follow-up decision |

## Ambiguity Register

| Topic | Evidence of ambiguity | Resolution / current stance |
| --- | --- | --- |
| Current implementation vs target scope | `docs/Project_Documentation.md` is broader than code | treat `docs/Implementation_Status.md` plus code/tests as present truth and the broader doc as target scope |
| `Sale` vs `Sales` naming | feature table says `Sale`, API/docs use `sales`, schema uses `SaleOrder` | normalize outward-facing docs/plans to `Sales`; keep current schema/code identifiers unchanged |
| `contact` vs `client` vs `customer` | management docs and API tables vary | normalize around `Contact` as canonical storage, with role/type labels in the UI |
| Registration policy | login dialog exposes registration, target docs say admin controls users | keep current registration path as unresolved legacy behavior and do not expand it |
| Company scope | target docs include companies, but the repo has no shared entity or table | keep companies explicitly out of implemented scope until fields/contracts are defined |
| Barcode and invoice formats | docs mention PDF, XML, XLSX, Medoc export, and uploads without concrete contracts | do not invent formats; keep these as unresolved product gaps |

## Terminology Normalization

| Canonical term | Alternatives found | Working rule |
| --- | --- | --- |
| `Sales` | `Sale`, `Sale invoices`, `SaleOrder` | use `Sales` for module/docs/API language; keep `SaleOrder` as the current schema/code identifier |
| `Contact` | `client`, `customer`, `contact` | use `Contact` as canonical storage; customer/client are business labels |
| `Supplier catalog` | `supplier's products base`, `pricelist upload` | use `supplier catalog` for the mapping/import slice backed by `SuppliersProductInfo` |
| `Inventory` | `stock tracking`, `inventory view` | reserve `Inventory` for stock/quantity records; `ProductType` is catalog/master data |
| `Logs` | `history`, `user logs`, `activity logs` | treat all as the current audit/log viewing requirement |
| `Language: en/ua` | `English/Ukrainian`, `uk/ua` | persist `en` and `ua`; load Ukrainian resources when `ua` is selected |

## Current-vs-Target Scope Matrix

| Module | Target scope summary | Current repo reality | Main gap |
| --- | --- | --- | --- |
| Users | login, user CRUD, role CRUD, profile editing, history | login + user CRUD + role CRUD implemented | self-profile flow and clearer registration policy |
| Settings/Localization | not explicit in target doc, now required by branch scope | persisted desktop settings and EN/UA selection implemented | no live language hot-swap |
| Inventory | Product Types, supplier product base, stock tracking | ProductType CRUD, supplier catalog mappings/CSV import, and stock tracking implemented | barcode labels, invoice-driven imports, multi-storage/company support |
| Purchase | orders, order records, invoices, barcode flows | orders + records + goods receipt posting implemented | incoming invoices/attachments and barcode flows |
| Sales | sales invoices, export, customer flows | orders + records + simple text export implemented | richer export formats and inventory posting policy |
| Management | employees, suppliers, contacts/customers, companies | contacts + suppliers + employees implemented | companies missing |
| Analytics | sales and inventory reports | baseline summary endpoints/views implemented | advanced analytics/report exports |
| Logs | user/activity history | audit writes and read-only browsing implemented | richer filtering/export/history structure |

## Requirements-Gap Matrix

| Requirement | Evidence | Current status | Gap / action |
| --- | --- | --- | --- |
| Consumer-agnostic server API | repo instructions, HTTP server design | implemented for current resources | keep future additions resource-oriented and client-neutral |
| Clean architecture boundaries | `_client`, `_server`, `_common` separation | maintained across the new slices | preserve discipline for remaining gaps |
| Desktop settings and language selection | current user request and branch goal | implemented | no live language hot-switch |
| Management master data | target docs + schema | implemented for contacts, suppliers, employees | add companies if/when fields are defined |
| ProductType CRUD | target docs + roadmap evidence | implemented | add barcode/label workflows later |
| Stock tracking | target docs + `Inventory` table | implemented | add multi-company storage/inventory operations later |
| Supplier product import | target docs + `SuppliersProductInfo` table | partially implemented | desktop CSV import is done; XML/XLSX/invoice-driven import remains |
| Purchase CRUD | target docs + schema | implemented baseline | add invoice registration/upload |
| Sales CRUD/export | target docs + schema | implemented baseline | add richer export formats and decide stock posting |
| Logs/audit views | target docs + `Log` table | implemented baseline | add richer filtering/export if justified |
| Analytics/reporting | target docs | implemented baseline | expand beyond current summary metrics |
| Release workflow | `Jenkinsfile`, docs build scripts | partial | define a real packaged release format |

## Per-Module Gap Analysis

| Module | Current evidence | Strengths already present | Missing pieces | Recommended next step |
| --- | --- | --- | --- | --- |
| Users | real server module, repositories, client MVVM/UI, tests | mature baseline slice | self-profile flow, registration-policy cleanup | preserve; do not destabilize |
| Settings/Localization | startup loading, persisted settings, Settings UI, translation asset | practical EN/UA baseline | live language switching test/support | keep restart-based application unless live reload is fully justified |
| Inventory | ProductType, stock, supplier catalog mappings, desktop CSV import | strongest data-management area in the repo | barcode labels, invoice-driven imports, company-aware storage | deepen only when missing contracts are defined |
| Purchase | real routes, repositories, receipt posting, desktop workflow | coherent vertical slice with stock integration | invoice upload/attachment and barcode workflows | add only from clarified product requirements |
| Sales | real routes, repositories, desktop workflow, simple export | coherent order/invoice baseline | richer export formats, stock effects, print pipeline | clarify target export formats before broadening |
| Management | contacts, suppliers, employees fully wired | solid supporting master-data slice | companies | add shared entity/schema only with justified field set |
| Analytics | sales and inventory summary endpoints/views | lightweight but real reporting baseline | richer metrics, charts, export/report persistence | expand after missing business decisions are clarified |
| Logs | audit writes plus read-only browsing | honest cross-cutting history baseline | richer filtering/export/history partitioning | add only if operational needs justify it |

## Recommended Implementation Priorities

1. Add interactive fullstack GUI/system validation for the implemented ERP workspaces.
2. Finalize and implement company-aware management/storage if the business fields are defined.
3. Define incoming invoice, barcode, and richer invoice export contracts before implementing them.
4. Package the current baseline into a real release format instead of archived binaries only.

Longer-horizon sequencing, architecture direction, and design-system guidance live in
`docs/Future_Architecture_and_Design_Roadmap.md`, where the next steps are organized as:

1. baseline hardening and release readiness
2. master data and inventory truth
3. procurement completion and ingestion workflows
4. sales fulfillment and document outputs
5. analytics, operability, and platform extension

## Architecture Diagram Set

- [System context](architecture/plantuml/System_Context.puml)
- [High-level use cases](architecture/plantuml/ERP_Use_Cases.puml)
- [Domain/module map](architecture/plantuml/Domain_Module_Map.puml)
- [Repository package map](architecture/plantuml/Repo_Package_Map.puml)
- [Runtime/deployment topology](architecture/plantuml/Runtime_Deployment.puml)
- [Validation flow](architecture/plantuml/Validation_Flow.puml)
- [Client component diagram](client/plantuml/Client_Design.puml)
- [Client class diagram](client/plantuml/GUI_CD.puml)
- [Client networking diagram](client/plantuml/Networking_CD.puml)
- [Server design](server/plantuml/Server_Design.puml)
- [Database schema](server/plantuml/DB_schema.puml)
- [ProductType request flow](inventory/plantuml/ProductType_Request_Flow.puml)
- [ProductType activity](inventory/plantuml/ProductType_CRUD_Activity.puml)
- [ProductType lifecycle state](inventory/plantuml/ProductType_Lifecycle_State.puml)
- [ProductType release workflow](inventory/plantuml/ProductType_Release_Workflow.puml)
