# SageStore Debug CLI Design

## Purpose

`SageStoreDebugCli` is a Codex-facing automation and diagnostics tool for SageStore.
It is intended to:

- trigger real server requests against the canonical SageStore HTTP API
- run the real Qt client stack offscreen against a chosen server
- drive covered UI events through stable object names and menu actions
- emit machine-readable JSON results for agent consumption
- save screenshots and structured UI state when a flow succeeds or fails

The tool is diagnostic infrastructure, not a replacement for the shipped client.

## Current Repo Reality

The repository already contains the core pieces needed for a reliable implementation:

- stable server endpoint constants in `_common/include/common/Endpoints.hpp`
- a real client networking/UI stack in `_client/src`
- offscreen Qt workflow coverage in `_client/tests`
- server process harness patterns in `_client/tests/include/wrappers/FullstackServerHarness.hpp`
- stable widget object names across dialogs, actions, views, tables, and controls

The repository also contains an early `tools/inspector` Go prototype, but it is not a sufficient base for the requested capability today:

- it targets placeholder or outdated routes such as `/debug/info`
- it is centered on a local web UI rather than a machine-oriented CLI contract
- it has no practical bridge into the real Qt widget tree for screenshots or UI state dumps
- it sits outside the current CMake and Qt test workflow

For those reasons, the first implementation is a C++/Qt executable built next to the existing client runtime. That keeps the UI automation in the same language and process model as the real client.

## Chosen Architecture

The tool is a single executable with two automation surfaces:

1. Server surface
- synchronous HTTP requests to canonical SageStore endpoints
- optional local server process orchestration for isolated runs
- JSON dataset request/response support

2. Client surface
- offscreen `QApplication` runtime
- real `NetworkService`, `ApiManager`, `DialogManager`, `StartupController`, and `MainWindow`
- covered UI actions and waits performed against stable object names
- widget tree serialization and screenshot capture

The primary workflow is scenario-based:

1. Codex writes a JSON scenario file.
2. `SageStoreDebugCli run --scenario scenario.json --result result.json --artifact-dir out/`
3. The tool executes steps in order.
4. The tool emits structured results plus any artifacts.
5. Codex inspects `result.json` and any screenshots/state dumps to diagnose or validate behavior.

## Command Contract

The first version exposes two top-level commands:

1. `describe`
- Prints the supported step types, known menu action names, and high-level result schema as JSON.
- This keeps the contract discoverable for Codex and future wrappers.

2. `run`
- Executes a JSON scenario.
- Writes the final result JSON to stdout by default.
- Optionally writes the same JSON to `--result`.
- Uses `--artifact-dir` as the default location for screenshots/state dumps and automatic failure artifacts.

## Scenario Contract

Scenario documents are JSON objects with optional `server`, `client`, and `steps` sections.

High-level shape:

```json
{
  "server": {
    "mode": "spawn",
    "address": "127.0.0.1",
    "port": 0
  },
  "client": {
    "autoAcceptDialogs": true
  },
  "steps": [
    { "action": "server.health" },
    { "action": "client.start" },
    { "action": "client.login", "username": "admin", "password": "admin123" },
    { "action": "ui.triggerMenu", "menu": "PRODUCT_MANAGEMENT" },
    { "action": "ui.waitTableRows", "objectName": "productTypesTable", "minRows": 1 },
    { "action": "ui.capture", "objectName": "mainWindow", "screenshotPath": "product-management.png", "statePath": "product-management.json" }
  ]
}
```

## Covered Actions

The first implementation deliberately supports a bounded but useful action set.

Server actions:

- `server.spawn`
- `server.health`
- `server.request`

Client lifecycle actions:

- `client.start`
- `client.login`
- `client.stop`

UI actions:

- `ui.triggerMenu`
- `ui.triggerAction`
- `ui.click`
- `ui.setText`
- `ui.setComboText`
- `ui.selectTableRow`
- `ui.editTableCell`
- `ui.waitVisible`
- `ui.waitText`
- `ui.waitTableRows`
- `ui.capture`
- `ui.dumpState`
- `ui.sleep`

Each step returns a JSON object with:

- `index`
- `action`
- `ok`
- `message`
- `result`

## Artifact Model

The tool generates two artifact classes:

1. Screenshot
- PNG captured with `QWidget::grab().save(...)`
- target is either an explicitly named widget or the current root window

2. UI state dump
- JSON serialization of visible top-level windows and the selected root subtree
- includes object names, class names, visibility, enabled state, geometry, and widget-specific state
- includes useful structured fields for tables, combo boxes, tabs, labels, line edits, and plain text editors

When a UI step fails and `--artifact-dir` is set, the tool automatically captures:

- `failure-<step-index>.png`
- `failure-<step-index>.json`

This makes failure diagnosis deterministic and Codex-friendly.

## Phase Plan

### Phase 1: Foundation

- executable target and argument parsing
- JSON scenario loading
- server request runner
- result JSON writer
- `describe` command

### Phase 2: Client Bridge

- offscreen Qt runtime
- startup/login orchestration through real dialogs/controllers
- stable widget lookup and covered UI actions
- waits and action result reporting

### Phase 3: Diagnostics And Validation

- widget tree dump
- screenshot capture
- automatic failure artifacts
- end-to-end system test against the real server and client UI
- status/documentation updates

## Compatibility And Limits

- No server contract changes are required.
- No DB schema changes are required.
- The tool is additive and does not alter the shipped client path.
- UI automation is intentionally limited to the covered action set; the scenario contract is extensible but not open-ended.
- Offscreen validation is diagnostic and regression-focused. It does not claim to replace interactive desktop signoff.

## Validation Strategy

The implementation is considered complete only if all are true:

- the new executable builds under the normal CMake flow
- the tool can spawn or target a real server
- the tool can drive a real client login and at least one real workspace flow
- the tool can emit JSON result data, a UI state dump, and a screenshot
- an automated test validates the tool against a real server-backed offscreen run

## PlantUML Views

- [Architecture](plantuml/Debug_CLI_Architecture.puml)
- [Run Flow](plantuml/Debug_CLI_Run_Flow.puml)
