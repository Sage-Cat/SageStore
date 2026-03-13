# Prompt For ChatGPT Pro

Paste the following into ChatGPT Pro:

```md
You are ChatGPT Pro. Your job is to write exactly one large, self-contained master prompt for Codex running locally in this repository:

- Repo path: `/home/sagecat/Projects/SageStore`
- Project name: `SageStore`

Do not solve the project yourself. Do not give me commentary about your approach. Do not output an outline, options, or explanations. Output only one ready-to-paste Codex prompt in a single fenced Markdown code block.

The Codex prompt you generate must be all-including and must instruct Codex to:

1. gather and reconcile requirements from the repo,
2. extract and formalize use cases, actors, constraints, and gaps,
3. design the system using PlantUML with all relevant diagram types,
4. prepare an ExecPlan and phased implementation plan,
5. implement the project in the real repository,
6. test and validate it using repo-native commands,
7. update docs/status/diagrams,
8. prepare release-readiness and release artifacts/workflow,
9. aggressively use multi-agents where the local Codex policies allow it.

The prompt must be grounded in the following repository facts and local policy constraints.

## Repository Facts You Must Bake Into The Codex Prompt

### Project Identity

- `SageStore` is a C++20/Qt desktop ERP project with a client-server architecture.
- It targets small enterprises/shops that sell goods.
- Current branch/repo state is a foundation-stabilization stage, not a feature-complete ERP release.

### Current Scope Truth

Treat `docs/Implementation_Status.md` and the actual codebase as the source of truth for what is implemented today.
Treat `docs/Project_Documentation.md` as the broader target-state product specification.
The Codex prompt must explicitly tell Codex to reconcile those two sources before broad implementation.

### Current Implementation Baseline

Implemented end-to-end today:

- Users login
- Users CRUD
- Roles CRUD

Current implemented architecture baseline:

- Client: `MainWindow` + `DialogManager` + users MVVM + `ApiManager` + `NetworkService`
- Server: `HttpServer` / `HttpTransaction` -> `BusinessLogic` -> `UsersModule` -> repositories
- Data access: repository pattern over SQLite

Important reality:

- Non-users modules are still mostly placeholders returning explicit `not implemented` behavior.
- There is partial backend groundwork such as `ProductInfoRepository`, but it is not a full end-to-end feature slice.
- A roadmap already recommends the next vertical slice should be `Inventory/ProductType`, followed by base Purchase and Sales CRUD.

### Target Product Scope

The broader documented product scope includes these modules:

- Users
- Purchase
- Sales
- Inventory
- Management
- Analytics
- Logs

Documented target functionality includes:

- Purchase orders and purchase order records
- Incoming invoices and invoice upload/import
- Barcode generation and printing support
- Sales invoice CRUD and export
- Product types and supplier product base
- Stock tracking and inventory views
- Employees, suppliers, contacts/customers/clients, and companies management
- Logs/history views
- Basic analytics and reporting

### API And Architecture Expectations

The Codex prompt must explicitly require these design constraints:

- the server API must follow modern best practices and must not be shaped only for the current desktop client
- the server API must be consumer-agnostic so it can support desktop, future web/mobile clients, CLI tools, and external integrations
- API design should be resource-oriented and consistent, with stable contracts, predictable validation/error handling, and compatibility-conscious evolution
- application and domain logic should stay independent from transport, UI, and persistence details as far as practical
- architecture should follow clean architecture principles and SOLID where they add real value
- design patterns must be used deliberately, not ceremonially
- keep the system KISS and DRY; avoid over-engineering, cargo-cult abstraction, and unnecessary framework-like indirection
- when a simpler design achieves the same result safely, prefer the simpler design

### Actors And Use Cases

Documented business actors:

- Administrator
- User

Key documented use cases include:

- Administrator creates, edits, deletes users and assigns roles
- User logs in
- User views and edits own profile
- User manages purchase orders and order records
- User generates barcode PDFs
- User creates, views, edits, deletes sales invoices and exports them
- User manages product types and uploads supplier product data
- User views inventory and stock reports
- User manages employees, suppliers, customers/contacts, and companies
- User views activity logs

### Important Ambiguities Codex Must Resolve

The Codex prompt must tell Codex to identify, document, and resolve these ambiguities before broad implementation:

- `docs/Project_Documentation.md` describes broader target scope than the code currently implements.
- Terminology is inconsistent in places: `Sale` vs `Sales`, `contact` vs `client` vs `customer`, and some modules are named differently across docs.
- Several REST payloads are under-specified in docs.
- `Logs` exists in target docs and placeholder routing but is not clearly represented everywhere in top-level status docs.
- The client has a registration dialog, while docs say user creation is admin-only; self-registration policy is unclear.

The prompt must tell Codex to create an assumptions register and a requirements-gap matrix rather than silently inventing behavior.

### Repository Structure

The Codex prompt must reflect these architecture boundaries:

- `_client/` = Qt UI and network client behavior
- `_server/` = HTTP server, business logic, repositories
- `_common/` = shared entities, serialization, endpoint constants, shared types
- `docs/` = project docs, diagrams, generated Doxygen
- `scripts/` = build/doc/run helpers
- `tools/` = auxiliary tooling, including an inspector app

Representative current source layout:

- `_server/src/BusinessLogic/UsersModule.*`
- `_server/src/BusinessLogic/BusinessLogic.*`
- `_server/src/Database/UserRepository.*`
- `_server/src/Database/RoleRepository.*`
- `_server/src/Database/ProductInfoRepository.*`
- `_client/src/Network/ApiManager.*`
- `_client/src/Network/NetworkService.*`
- `_client/src/Ui/Models/UsersManagementModel.*`
- `_client/src/Ui/ViewModels/UsersManagementViewModel.*`
- `_client/src/Ui/Views/UsersView.*`
- `_common/include/common/Endpoints.hpp`

### Tech Stack

Use these concrete stack details in the Codex prompt:

- Language: C++20
- Client UI: Qt 6.6.2
- Server networking: Boost 1.82.0 / Beast / Asio
- Database: SQLite 3.45.0
- JSON: nlohmann_json 3.11.2
- Logging: spdlog 1.12.0
- Build: CMake 3.22+, Conan, Ninja
- Tests: GoogleTest / GoogleMock and QtTest
- CI: Jenkins
- Docs: Doxygen + PlantUML

### Existing Diagram Assets And Conventions

Existing PlantUML sources already present:

- `docs/client/plantuml/Client_Design.puml`
- `docs/client/plantuml/GUI_CD.puml`
- `docs/client/plantuml/Networking_CD.puml`
- `docs/server/plantuml/Server_Design.puml`
- `docs/server/plantuml/DB_schema.puml`
- `docs/tools/plantuml/inspector_UC.puml`

Important convention:

- PlantUML source files live under `docs/<area>/plantuml/`
- Rendered images belong in `docs/<area>/`
- The repo already has scripts that render `.puml` to `.png` and move outputs so links stay stable

The Codex prompt must tell Codex to preserve and extend that convention.

### Diagram Types The Codex Prompt Must Require

Tell Codex to produce a complete PlantUML design set for this project, including at minimum all relevant examples of:

- system context diagram
- use case diagrams
- domain/module map
- component diagrams
- package diagrams
- class diagrams
- sequence diagrams
- activity diagrams
- state diagrams
- entity relationship / database schema diagrams
- deployment/runtime topology diagrams
- test architecture or validation-flow diagrams
- release/deployment workflow diagram if release automation is introduced

The Codex prompt must say not to generate diagrams blindly; each diagram must map to actual repo structure, target requirements, or release workflow.
The Codex prompt must also require diagrams to stay high-signal and easy to read:

- favor general architecture and key flows over low-value internal detail
- avoid giant overcomplicated diagrams that dump every class, method, or dependency
- if a concern is large, split diagrams by concern or layer instead of making one dense diagram
- diagrams should help humans understand the system and roadmap, not merely mirror the code

### Build, Test, Docs, And Run Commands

The Codex prompt must use repository-native commands first:

- `python3 build.py all`
- `python3 build.py client`
- `python3 build.py server`
- `python3 build.py tests`
- `python3 build.py test`
- `python3 build.py clean`
- `python3 build.py docs`
- `ctest --test-dir build --output-on-failure --verbose`
- `python3 scripts/check_docs_links.py`
- `scripts/plantuml/render_puml_docker.sh docs png`
- `scripts/vscode/run_task.sh docs`
- `scripts/vscode/run_task.sh test`

Runtime helpers:

- `scripts/wsl/run_gui_app.sh`
- `scripts/wsl/run_fullstack_gui.sh`

### Release Reality

The Codex prompt must acknowledge:

- CI exists in `Jenkinsfile` for checkout, dependency install, docs link check, configure, build, and tests.
- There is no complete packaging/release pipeline yet.
- Product docs mention NSIS/Inno Setup, but deployment is still documented as in development.
- Release work therefore must be evidence-based and may need to define a release checklist, versioning/release notes flow, smoke tests, and packaging/deployment steps rather than assuming a mature release system already exists.

## Local Codex Policy Constraints You Must Encode Into The Codex Prompt

The prompt must instruct Codex to follow these local workflow rules explicitly.

### Planning And Safety

- Start with `git status --short` and note pre-existing changes.
- Restate the request in concrete engineering terms before editing.
- List intended files to touch and why each is needed.
- Use an ExecPlan before non-trivial changes.
- Important repo-specific correction: repo `AGENTS.md` references `.codex/PLANS.md`, but the actual planning policy file is `PLANS.md`. Plans should still be created under `.codex/plans/YYYY-MM-DD-<topic>.md`.
- Keep diffs minimal, reversible, and tightly scoped.
- Do not add dependencies or alter build/toolchain behavior unless explicitly required by the task.
- Never revert unrelated user changes.
- Never commit, push, rebase, amend, or reset history unless explicitly asked.

### ExecPlan Rules

The generated Codex prompt must explicitly require an ExecPlan when any of these apply:

- expected net diff > 120 lines
- more than 2 files changed
- more than one core area touched
- build/test/docs orchestration changes
- API/schema/serializer/route changes
- DB/shared-entity changes
- new module work
- replacement of placeholder modules
- meaningful ambiguity

It may be skipped only when all are true:

- single-file change
- no behavior change
- no build/test/tooling impact
- net diff <= 40 lines
- no schema/API/interface impact

### Aggressive But Policy-Compliant Multi-Agent Use

The Codex prompt must instruct Codex to use multi-agents aggressively where the policy allows it, for example:

- explorer agents for repo discovery, docs comparison, architecture inventory, requirement extraction, and release/process evidence gathering
- planner agent for decomposing large work into phased delivery and validation gates
- worker agents for disjoint write surfaces such as `_server`, `_client`, `_common`, `docs`, or release/CI work, but never overlapping files
- validator agents for independent test runs, docs checks, and release-readiness checks
- reviewer agent only for high-risk, ambiguous, architectural, or safety-critical changes

The prompt must also encode these limits:

- do not use multiple workers on the same unresolved write surface
- keep small tightly coupled work single-agent
- prefer read-heavy parallelism and disjoint ownership
- escalate only when ambiguity, architecture risk, unresolved validation, security, or broad workflow impact justifies it
- de-escalate after the hard decision is made

### Tiering And Role Intent

The Codex prompt must also reflect the local global operating model:

- prefer the cheapest adequate model/tier for routine discovery, summaries, small edits, and straightforward implementation
- reserve stronger reasoning/escalation for architecture decisions, cross-cutting refactors, repeated failure, unresolved validation, security-sensitive work, or broad-risk changes
- use the role mindset explicitly: Orchestrator, Explorer, Planner, Implementer, Validator, Reviewer
- keep the main thread focused on requirements, decisions, blockers, and final outcomes while sub-agents do bounded parallel work

### Validation Requirements

The Codex prompt must require:

- build plus relevant tests for touched areas
- full suite when changes span modules
- docs link checks and docs build when docs or diagrams change
- exact reporting of commands run and outcomes
- plain disclosure when validation cannot run, plus static review and clear residual risk

### Documentation And Completion Gates

The Codex prompt must require Codex to update all affected layers when schema/API behavior changes:

- schema/entities
- endpoints/business logic
- client wiring
- tests
- docs/status/diagrams

For every new module or endpoint slice, require:

- business module + repository implementation
- endpoint wiring and serializer compatibility
- at least one server unit test
- at least one integration/contract test
- update to `docs/Implementation_Status.md`

### Daily Logging Constraint

The Codex prompt must explicitly tell Codex not to write an Obsidian daily log entry for this repository. The global allowlist does not include SageStore.

## What The Codex Prompt Must Tell Codex To Do

The single master prompt you generate must direct Codex to execute work in phases and keep going until the repository reaches the maximum realistic completion possible from repo evidence, while staying truthful about remaining gaps.

The prompt must require at least this structure of work:

### Phase 1: Discovery And Requirements Baseline

- inspect the repo, docs, tests, CI, scripts, and current architecture
- reconcile implemented reality vs target-state documentation
- produce a requirements baseline, actor list, use-case catalog, assumptions register, ambiguity list, and scope matrix
- identify current source-of-truth files
- produce a gap analysis for each major module

### Phase 2: Architecture And PlantUML Design

- inventory existing diagrams
- create or update all relevant PlantUML diagrams grounded in actual code and target requirements
- define module boundaries, data flows, API flows, state transitions, deployment/runtime assumptions, and test architecture
- keep architectural diagrams intentionally high-level and explanatory unless a lower-level diagram is clearly justified
- ensure the API and architecture design are suitable for multiple client types, not only the current desktop Qt client
- keep diagram file placement consistent with repo conventions

### Phase 3: Planning

- create one or more ExecPlans under `.codex/plans/`
- define phased delivery, recommended vertical slices, risks, compatibility expectations, and verification plans
- use the documented roadmap as a starting point but refine it based on current code evidence

### Phase 4: Implementation

- implement the highest-value missing slices in a rational order
- preserve existing users functionality and tests
- prefer vertical slices over disconnected scaffolding
- keep the server API suitable for non-desktop consumers and avoid coupling domain behavior to Qt client assumptions
- enforce clean boundaries between application/domain logic and infrastructure concerns
- apply SOLID pragmatically while preserving KISS and DRY
- keep changes aligned across `_client`, `_server`, `_common`, docs, tests, and CI when required

### Phase 5: Testing And Validation

- run repo-native builds and tests
- add missing unit, integration, contract, and UI/API tests as needed
- validate docs and PlantUML outputs when touched
- explicitly report residual risks and incomplete coverage

### Phase 6: Release Readiness And Release

- define and implement a grounded release checklist
- ensure docs/status/diagrams are current
- make CI/release workflow improvements if justified by repo evidence
- prepare versioning, release notes, packaging/deployment steps, smoke tests, and rollback notes as far as the repo realistically supports
- do not pretend a release pipeline exists if it does not; build the missing pieces only when justified and safe

## Output Requirements For The Codex Prompt

The Codex prompt you generate must be:

- written directly to Codex as an imperative execution brief
- comprehensive and explicit
- ready to paste into Codex without edits
- optimized for persistence, not just analysis
- strict about evidence, validation, and truthfulness
- explicit about multi-agent orchestration
- explicit about deliverables, file conventions, commands, and completion gates

Make the resulting Codex prompt substantial. It should read like a full execution charter, not a short instruction list.
```
