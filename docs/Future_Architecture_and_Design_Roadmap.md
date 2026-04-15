# SageStore Future Architecture and Design Roadmap

## Purpose

This document defines the forward-looking product, architecture, and UX direction for
SageStore beyond the current validated MVP baseline.

Use it together with the other top-level docs:

- `docs/Implementation_Status.md` for present implementation truth
- `docs/Requirements_Baseline.md` for current-vs-target reconciliation
- `docs/Project_Documentation.md` for broad target product/reference design

This roadmap is directional. It is meant to guide sequencing and design quality, not
to promise that every item is already funded or fully specified.

## Product North Star

SageStore should evolve as a desktop-first, API-backed ERP for small goods-selling
operations that need:

- trustworthy master data
- clean purchasing and sales workflows
- auditable inventory movement
- practical reporting
- low operational overhead

The current architecture already supports that trajectory. The goal is to deepen the
existing modular monolith and desktop workflow model, not to replace it with a
premature rewrite.

## Planning Assumptions

- The baseline remains a modular monolith with a Qt desktop client, a consumer-agnostic
  HTTP server, and SQLite persistence.
- The next release-quality gains come from stronger workflow integrity, test depth,
  packaging, and operability rather than from broad feature expansion alone.
- ERP sequencing matters: master data and stock truth should settle before aggressive
  document automation, and document automation should settle before advanced analytics
  or multi-channel expansion.
- New architecture patterns should be introduced only when they reduce product risk or
  operator friction for real workflows.

## Guiding Principles

### 1. Modular monolith first

- Keep `_server` as a well-bounded modular monolith until scale, deployment, or team
  structure proves otherwise.
- Prefer clearer module seams and better contracts over microservice extraction.
- Add new business areas as modules with explicit ownership rather than as cross-cutting
  helpers.

### 2. Contract-first, consumer-agnostic APIs

- Preserve the current HTTP API posture as resource-oriented and reusable beyond the
  Qt desktop client.
- Keep request/response contracts additive where possible.
- Avoid leaking Qt-specific assumptions, UI formatting, or view-level behavior into the
  shared API surface.

### 3. Workflow-centered business logic

- Treat purchase, inventory, sales, and future document flows as explicit business
  workflows with well-defined state transitions.
- Keep invariants in server-side business modules instead of duplicating them across
  endpoints, views, or repositories.
- Use command-style actions only for meaningful workflow transitions such as posting a
  receipt or finalizing an export.

### 4. Master data before automation

- Deepen master data quality before adding aggressive automation.
- Companies, storages, contacts, suppliers, product types, and supplier product
  mappings should be modeled clearly before large import/export workflows are expanded.
- Avoid building invoice-import or label-generation flows on unstable identifiers.

### 5. Imports and exports as pipelines

- Treat imports, exports, and label/document generation as staged pipelines:
  parse, validate, preview, commit, audit.
- Long-running work should move into background jobs with visible status rather than
  blocking the main UI thread.
- Keep file-format adapters isolated from core transaction logic.

### 6. Reporting from read models

- Keep transactional writes simple and authoritative.
- Start with query-oriented views over the existing SQLite model, then introduce
  read-optimized projections or cached summaries only when reporting complexity demands
  it.
- Avoid mixing reporting-specific denormalization into the write path too early.

### 7. Operability and audit by default

- Every business mutation should remain explainable through audit data, logs, and
  reproducible workflow states.
- Favor correlation-friendly logs, import/export job history, validation messages, and
  clear operational runbooks.
- Release quality should include packaging, smoke validation, backup/restore guidance,
  and upgrade discipline, not only feature completion.

## Architecture Evolution Directions

### Domain boundaries

- Preserve the current module split of `users`, `inventory`, `management`, `purchase`,
  `sales`, `logs`, and `analytics`.
- Introduce new modules only when the business language is stable enough to justify
  them. Likely future candidates are:
  - `companies`
  - `documents` or `attachments`
  - `integration/jobs`
- Keep repositories focused on persistence concerns. Cross-entity workflow rules should
  stay in business modules.

### Modern ERP operating model

- Keep a clear split between master data, transactional documents, and read/reporting
  views.
- Treat purchase orders, goods receipts, sales orders, invoice registration, and future
  stock adjustments as business documents with explicit lifecycle rules.
- Prefer command/query separation at the workflow boundary:
  - commands perform state-changing business transitions with audit
  - queries power lists, dashboards, filters, previews, and reporting views
- Move toward a stock-movement model that explains why quantity changed, not only what
  the latest quantity is.
- Keep document attachments, import files, and export artifacts outside the core domain
  entity shapes whenever possible; reference them through workflow services and audit
  metadata.

### Transactional lifecycle modeling

- Formalize document lifecycles instead of relying on loose status strings.
- Purchase, receipt, sales, stock-adjustment, and invoice-registration flows should each
  define allowed transitions, side effects, and audit expectations.
- Sales stock posting should be a conscious workflow decision, not an incidental side
  effect added ad hoc to the UI.

### Company and storage model

- Add `Company` only when its ownership rules, reporting impact, and relationship to
  stock are defined.
- Model storage locations as operational inventory containers, not as a substitute for
  company boundaries.
- Introduce company-aware inventory only when the system can explain which records are
  global, company-scoped, or storage-scoped.

### Import, export, and document processing

- Split document processing into adapters and domain services:
  - format adapters for CSV/XML/XLSX/PDF/XML export targets
  - validation/staging services
  - commit/posting services
- Add background job handling for:
  - supplier catalog imports
  - invoice imports
  - barcode/label generation
  - large report/export generation
- Design every import/export path for idempotency, preview, and traceability.

### Reporting and analytics

- Keep the current analytics slice lightweight, but prepare for richer read models.
- Use reporting terminology that distinguishes operational lists from analytical views.
- Add drill-down, saved filters, and time-based summaries before considering a separate
  analytics subsystem.

### Observability and operational quality

- Strengthen mutation audit coverage and make it easier to trace a workflow across UI,
  API, and server module boundaries.
- Add health/readiness checks, import/export job status visibility, and clearer operator
  error messages.
- Keep release automation focused on practical deployability: packaged binaries,
  configuration defaults, smoke checks, and migration-safe schema changes.

## Phased Delivery Plan

### Phase 1: Baseline hardening and release readiness (`0-3 months`)

#### Goals

- make the current ERP baseline safer to ship and easier to validate
- close the highest-value quality gaps before broadening scope

#### Focus areas

- interactive fullstack GUI/system coverage for implemented workspaces
- dedicated verification for sales invoice export output
- packaging and installer direction for a repeatable desktop release
- stronger error, audit, and docs consistency

#### Exit signals

- the main implemented workflows are exercised beyond startup smoke
- the project has one documented, repeatable packaging/release path
- current-state docs and operator runbooks no longer conflict

### Phase 2: Master data and inventory truth (`3-6 months`)

#### Goals

- close the most important business-entity and inventory-ownership gaps
- strengthen transaction integrity around stock before broadening document automation

#### Focus areas

- companies and storage/location modeling
- clarified ownership rules for `Contact`, `Supplier`, `Employee`, and future `Company`
- stock movement semantics for receipts, transfers, corrections, and future sales posting
- clearer status models for purchase and sales document lifecycles where stock is affected

#### Exit signals

- core business entities have stable ownership and scope rules
- stock movement has a coherent story across purchase, receipts, corrections, and sales
- future document and analytics work can build on stable identifiers and scopes

### Phase 3: Procurement completion and ingestion workflows (`6-9 months`)

#### Goals

- turn purchasing into a more complete procure-to-receive workflow
- make document ingestion and supplier-data processing explicit, testable workflows

#### Focus areas

- incoming invoice registration and attachment flows
- staged import pipelines with preview, validation, and audit
- background jobs for supplier catalog import, invoice processing, and label generation
- reconciliation between supplier documents and internal product mappings
- clearer operator feedback for partially successful or failed document-processing steps

#### Exit signals

- long-running document flows are observable, resumable, and testable
- procurement workflows no longer depend on manual side channels for core evidence
- import-related failures are diagnosable without digging through raw logs

### Phase 4: Sales fulfillment and document outputs (`9-12 months`)

#### Goals

- make sales operationally complete instead of only administratively present
- improve outbound business documents without overcommitting to speculative formats

#### Focus areas

- explicit policy and implementation for sales-to-inventory posting or reservation
- richer invoice outputs, starting with HTML/PDF before format-specific integrations
- printing/export workflow consistency
- stronger audit coverage around sales state changes and export actions
- clearer distinction between draft, posted, exported, and canceled document states

#### Exit signals

- sales operations no longer rely on ambiguous stock side effects
- document outputs have a repeatable rendering and verification story
- external-format work can proceed from explicit business targets instead of vague intent

### Phase 5: Analytics, operability, and platform extension (`12-18 months`)

#### Goals

- deepen operational support quality and reporting value after the ERP baseline is solid
- broaden the product without destabilizing the workflow foundations underneath it

#### Focus areas

- richer analytics, drill-down reporting, and saved views
- richer log filtering, export, and operational history
- backup, restore, and upgrade runbooks
- deeper observability and support diagnostics
- API refinements for future non-desktop consumers
- more flexible integration points for external systems
- ongoing design-system consolidation across all desktop workspaces

#### Exit signals

- the product can support broader reporting and integration use cases without a major
  architectural reset
- support and release operations have documented, repeatable procedures
- new consumers can be added by extending stable contracts rather than bypassing them

## Sequencing Rules

- Do not implement multi-company behavior before ownership and storage rules are clear.
- Do not expand invoice or barcode automation before master data identifiers are stable.
- Do not move to microservices to solve problems that can still be handled by explicit
  module boundaries.
- Do not add complex reporting infrastructure before the operational workflows and
  query needs prove it necessary.
- Do not introduce background jobs until the sync workflow contract, operator feedback,
  and audit path are clear.

## Future Design System Direction

### UX north star

The desktop client should feel like a serious operator workspace:

- dense enough for fast data work
- predictable enough to learn once and reuse everywhere
- forgiving enough to prevent accidental data corruption
- localized and accessible enough for everyday business use

The design system should optimize operator throughput and clarity, not decorative
minimalism.

### Interaction principles

#### 1. One shell, many workspaces

- Keep a consistent workspace shell with stable navigation, tab behavior, page titles,
  and action placement.
- New modules should fit the same mental model instead of inventing bespoke screens.

#### 2. List-detail by default

- Master-data screens should prefer a list/filter panel plus detail/editor workflow.
- Transaction screens should prefer a document header plus line-entry table plus totals
  summary.
- Avoid deep navigation trees when a two-level workflow is enough.

#### 3. Keyboard-first data entry

- Prioritize tab order, arrow-key movement in tables, default action shortcuts, and
  predictable focus behavior.
- Data-heavy screens should be efficient without requiring constant mouse use.

#### 4. Inline validation before modal interruption

- Show field-level and row-level validation near the error source.
- Reserve modal dialogs for confirmations, destructive actions, or short focused tasks.
- Use a consistent validation summary area for cross-field or document-level failures.

#### 5. Shared visual language for state

- Use the same status colors, labels, badges, and icons across purchase, sales,
  inventory, and background jobs.
- Distinguish editable, posted, failed, and pending states consistently.

#### 6. Localization and formatting as first-class concerns

- Centralize number, date, currency, and unit formatting.
- Keep labels, validation text, and status names translation-ready from the start.
- Avoid hard-coded mixed-language strings in views or models.

#### 7. Accessibility and readability

- Maintain reliable keyboard navigation, visible focus, adequate contrast, and scalable
  text/layout behavior.
- Support dense views without sacrificing legibility.
- Use tooltips and helper text where domain language is not self-evident.

### Design foundations

- Use a stable set of semantic tokens for spacing, density, typography, emphasis,
  validation, and status colors.
- Prefer shared helpers for field alignment, section spacing, table formatting, and
  message presentation over per-view styling tweaks.
- Treat localization, keyboard navigation, and focus visibility as core design-system
  requirements, not optional cleanup work.

### Component and workflow standards

#### Workspace shell

- common page header with title, context actions, and lightweight status feedback
- stable menu and tab naming
- consistent empty/loading/error states

#### Tables and lists

- reusable filter/search area
- consistent column formatting and alignment
- row double-click opens detail/editor
- optional totals or summary row for transactional grids
- preserve column widths/order only where it improves operator efficiency

#### Forms and editors

- predictable field grouping and section ordering
- clear required/optional/read-only states
- sticky save/cancel actions for long forms
- dirty-state awareness before closing or switching tabs

#### Transaction documents

- header section for document identity and counterparties
- editable lines grid with immediate row validation
- summary section for totals, posting state, and downstream actions
- visible audit-relevant state changes such as posted/received/exported

#### Notifications and errors

- inline errors for local validation
- toast or status-bar feedback for successful background-safe actions
- modal confirmations only for destructive or irreversible transitions
- backend errors mapped to readable operator language without hiding useful detail

### Design-system implementation backlog

1. Create shared Qt form/layout helpers for labels, field widths, and action rows.
2. Create a reusable data-grid wrapper for filtering, sorting, formatting, and keyboard
   behavior.
3. Standardize status badges, message styles, and validation summaries.
4. Centralize formatting and localization helpers for dates, money, units, and status
   names.
5. Define theme tokens for spacing, density, typography, and semantic colors.
6. Reduce view-specific one-off widget arrangements in favor of reusable patterns.

## Open Product Decisions

- Registration policy: keep self-registration, limit it, or move fully to admin-only
  provisioning.
- Company model: determine ownership boundaries for contacts, suppliers, employees,
  product types, and stock records before adding schema fields broadly.
- Storage model: decide whether storage is warehouse-style, location-style, or both.
- Sales inventory behavior: decide between reservation, decrement-on-post, decrement-on-
  fulfill, or a richer fulfillment workflow before expanding analytics and exports.
- Invoice/export targets: implement XML or external integration formats only when the
  target system and acceptance samples are explicit.
- Attachment/document storage: decide whether binary artifacts live in the database,
  filesystem, or a referenced external store before scaling document workflows.

## Risks to Avoid

- Premature microservices or event-driven complexity without real operational need.
- Allowing UI screens to orchestrate multi-step business workflows directly.
- Encoding file-format quirks inside shared entities or repositories.
- Adding company or storage fields everywhere without a stable ownership model.
- Building separate UX patterns for every module instead of a reusable operator
  workspace standard.
- Overusing modal dialogs for large forms or routine data-entry steps.
- Treating analytics and exports as UI-only conveniences instead of auditable workflows.

## Decision Gate for New Work

Before starting major new ERP slices, validate that:

- the business vocabulary is stable enough for shared entities and endpoint contracts
- workflow states and invariants are explicit
- audit and validation expectations are defined
- the change fits the existing modular architecture without a broad rewrite
- the client UX can reuse existing patterns or justifies a new one clearly
