# SageStore Requirements Baseline

## Snapshot

- Date: 2026-03-13
- Current-state source of truth: `docs/Implementation_Status.md` plus the codebase and tests
- Target-state source of truth: `docs/Project_Documentation.md`
- Supporting evidence: `README.md`, `Jenkinsfile`, `scripts/create_db.sql`, existing PlantUML assets, build/test scripts, and the actual module wiring under `_client/`, `_server/`, and `_common/`

## Reconciliation Summary

- The target product documentation still describes a broader ERP than the repository currently implements.
- Current end-to-end reality is:
  - Users login
  - Users CRUD
  - Roles CRUD
  - Inventory/ProductType CRUD through shared endpoint constants, server business logic/repository, client API/MVVM/UI, and automated tests
- The rest of the ERP remains planned or partial:
  - `purchase`, `sales`, `management`, `analytics`, and `logs` still dispatch to placeholder/not-implemented behavior in `BusinessLogic`
  - schema entities and some repository groundwork exist for later slices
- Release/process reality:
  - Jenkins validates checkout, Conan install, markdown links, configure, build, and `ctest`
  - PlantUML render and Doxygen generation exist locally, but are not enforced in Jenkins
  - packaging/publish automation is still absent

## Actors

| Actor | Type | Evidence | Current effective scope |
| --- | --- | --- | --- |
| Administrator | Business | `docs/Project_Documentation.md` user stories | Login, manage users, manage roles |
| User | Business | `docs/Project_Documentation.md` user stories | Login, manage Product Types; broader ERP use cases remain planned |
| Desktop client | Technical consumer | `_client/` implementation | Full consumer for current implemented slices |
| Future API consumer | Technical consumer | repo architecture constraints and current HTTP server design | Must be supported by resource-oriented, consumer-agnostic server contracts |
| Developer / CI | Delivery actor | `Jenkinsfile`, `build.py`, `scripts/` | Builds, tests, docs checks, release-readiness evidence |

## Use-Case Catalog

| Use case | Primary actor | Target doc status | Current repo status | Evidence |
| --- | --- | --- | --- | --- |
| Login | Administrator, User | Defined | Implemented | `UsersModule`, `ApiManager`, `DialogManager`, tests |
| Create/edit/delete users | Administrator | Defined | Implemented | `UsersModule`, `UserRepository`, `UsersManagement*`, tests |
| Assign roles | Administrator | Defined | Implemented | `UsersModule`, `RoleRepository`, tests |
| Edit own profile | User | Defined | Not implemented as a separate flow | no dedicated endpoint/UI flow |
| Manage Product Types | User | Defined | Implemented baseline | `InventoryModule`, `ProductTypeRepository`, `ProductTypes*`, tests |
| Search/filter Product Types | User | Defined | Not implemented | no filter/search path in server or client |
| Upload supplier product data | User | Defined | Not implemented | no end-to-end route/module/UI |
| View stock / inventory records | User | Defined | Not implemented | `Inventory` table exists, no slice |
| Manage purchase orders and records | User | Defined | Not implemented | schema exists, module/UI absent |
| Manage sales invoices and export | User | Defined | Not implemented | schema exists, module/UI absent |
| Manage employees, suppliers, customers/contacts, companies | User | Defined | Not implemented | schema exists, module/UI absent |
| View logs / history | User | Defined | Not implemented | `Log` table exists, route placeholder only |
| Run analytics / reports | User | Defined | Not implemented | no analytics module implementation |

## Assumptions Register

| ID | Assumption | Reasoning / evidence | Impact |
| --- | --- | --- | --- |
| A1 | `Inventory/ProductType` is the first non-users vertical slice to advance | shared entity, DB table, menu entry, and partial groundwork already existed | keeps scope realistic and end-to-end |
| A2 | The server API must stay consumer-agnostic and resource-oriented | repo instructions explicitly require modern API design beyond the desktop client | endpoint contracts stay additive and not Qt-specific |
| A3 | Self-registration is not a release requirement | target docs say user creation is admin-driven, while current registration dialog is a legacy implementation artifact | do not extend self-registration; treat it as unresolved legacy behavior |
| A4 | `Contact` is the canonical shared data model for customer/client-style parties | schema has `Contact`, while docs alternate between contact/client/customer | future management/sales work should normalize around one entity with type-based roles |
| A5 | `Sales` is the canonical module name for external API/docs, while `SaleOrder` remains the legacy schema/entity name | docs and routes use `sales`; code/schema use `SaleOrder` | avoid premature rename; document the mismatch instead |
| A6 | No schema migration is required for the ProductType slice | existing `ProductType` table supports the implemented fields | implementation stays additive and low-risk |
| A7 | The ProductType payload excludes `ukt_zed` for now | target doc mentions it, but the generated entity, schema, and JSON serializers do not | docs must call this out as a gap, not invent the field |

## Ambiguity Register

| Topic | Evidence of ambiguity | Resolution / current stance |
| --- | --- | --- |
| Current implementation vs target scope | `docs/Project_Documentation.md` is broader than code | treat `docs/Implementation_Status.md` plus code/tests as present truth and the broader doc as target scope |
| `Sale` vs `Sales` naming | feature table says `Sale`, API/docs use `sales`, schema uses `SaleOrder` | normalize outward-facing docs/plans to `Sales`; keep code/schema names unchanged until a dedicated rename is justified |
| `contact` vs `client` vs `customer` | management docs and API tables vary | normalize future requirements around `Contact` as canonical storage with business-type semantics |
| Logs module visibility | target docs define Logs, status docs historically under-represented it | explicitly track Logs as planned with schema + placeholder routing only |
| Registration policy | login dialog exposes registration, target docs say admin controls users | treat registration as a legacy bootstrap/testing flow and a release blocker until policy is resolved |
| ProductType payload shape | docs are not precise and mention fields absent from current schema | current implemented payload is `id`, `code`, `barcode`, `name`, `description`, `lastPrice`, `unit`, `isImported` |

## Terminology Normalization

| Canonical term | Alternatives found | Working rule |
| --- | --- | --- |
| `Sales` | `Sale`, `Sale invoices`, `SaleOrder` | use `Sales` for module/docs/API language; keep `SaleOrder` as current schema/code identifier |
| `Contact` | `client`, `customer`, `contact` | treat `Contact` as canonical storage; other labels are role-specific views |
| `ProductType` | `Product Type`, `product-types` | use `ProductType` for entity, `product-types` for the REST subresource |
| `Inventory` | `stock tracking`, `inventory view` | reserve `Inventory` for stock/quantity records; `ProductType` is catalog/master data |
| `Logs` | `history`, `user history`, `contacts history` | treat all as audit/log viewing requirements until split later |

## Current-vs-Target Scope Matrix

| Module | Target scope summary | Current repo reality | Main gap |
| --- | --- | --- | --- |
| Users | login, user CRUD, role CRUD, profile editing, history | login + user CRUD + role CRUD implemented | profile editing and dedicated history UI |
| Inventory | Product Types, supplier product base, stock tracking | ProductType CRUD implemented baseline | stock records, supplier upload/import, search/filter |
| Purchase | orders, order records, invoices, barcode flows | schema only / placeholder dispatch | first business slice not started |
| Sales | sales invoices, export, customer flows | schema only / placeholder dispatch | first business slice not started |
| Management | employees, suppliers, contacts/customers, companies | schema only / placeholder dispatch | no end-to-end master-data slice |
| Analytics | sales and inventory reports | placeholder only | no data pipeline or reporting endpoints |
| Logs | user/activity history | schema + placeholder only | no module, no UI, no search/filter |

## Requirements-Gap Matrix

| Requirement | Evidence | Current status | Gap / action |
| --- | --- | --- | --- |
| Consumer-agnostic server API | repo instructions, HTTP server design | addressed for current implemented resources | keep all future slices resource-oriented and client-neutral |
| Clean architecture boundaries | `_client`, `_server`, `_common` separation | broadly in place | maintain discipline as Purchase/Sales are added |
| ProductType CRUD | target docs + roadmap evidence | implemented | add deeper view/system coverage and search/filter |
| Stock tracking | target docs + `Inventory` table | not implemented | make this the next inventory follow-up after ProductType |
| Supplier product import | target docs + `SuppliersProductInfo` table | not implemented | define ingestion format and workflow |
| Purchase CRUD | target docs + schema | not implemented | next major transactional slice after inventory hardening |
| Sales CRUD/export | target docs + schema | not implemented | follow Purchase with shared contact/customer rules |
| Management master data | target docs + schema | not implemented | deliver after transactional baseline depends on it |
| Logs/audit views | target docs + `Log` table | not implemented | define log source and retrieval contracts |
| Analytics/reporting | target docs | not implemented | depends on transactional and inventory data maturity |
| Release workflow | `Jenkinsfile`, docs build scripts | partial | add docs-render enforcement, smoke gate, packaging notes |

## Per-Module Gap Analysis

| Module | Current evidence | Strengths already present | Missing pieces | Recommended next step |
| --- | --- | --- | --- | --- |
| Users | real server module, repositories, client MVVM, tests | mature baseline slice | self-profile flow, history/log view, stronger contract/UI coverage | preserve; do not destabilize |
| Inventory | ProductType entity/schema/menu now wired end-to-end | first non-users vertical slice complete enough to extend | stock records, search/filter, supplier upload, deeper GUI/system coverage | extend into Inventory stock records |
| Purchase | schema and target docs only | clear business scope and DB entities | routes, repositories, business logic, client UI | implement base order CRUD + records |
| Sales | schema and target docs only | clear adjacency to contacts/inventory | routes, repositories, business logic, client UI, export | implement base CRUD after Purchase |
| Management | schema and target docs only | master-data entities exist | routes, repositories, client UI, normalization around `Contact` | define Contact/Supplier/Employee first |
| Analytics | target docs only | reporting intent documented | no module, data aggregation, or UI | defer until transactional slices exist |
| Logs | schema + placeholder routing | audit concept exists in docs and DB | no module, no source events, no UI | define log capture strategy after core slices |

## Recommended Implementation Order

1. Keep `Inventory/ProductType` stable and add deeper view/system validation plus search/filter.
2. Extend Inventory into stock/inventory records so the catalog slice becomes operational.
3. Deliver Purchase base CRUD plus order records as the first transactional slice.
4. Deliver Sales base CRUD after Purchase, reusing normalized party/contact rules.
5. Add Management master data required by Purchase/Sales, then Logs, then Analytics.

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
