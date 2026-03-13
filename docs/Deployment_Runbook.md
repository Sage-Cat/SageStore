# SageStore Deployment Runbook

## Scope

This runbook covers the current honest deployment shape of SageStore on March 13, 2026:

- build the desktop client and server locally
- run the server with environment-based configuration
- launch the client against the configured server
- validate the installation with the inventory API smoke and the offscreen GUI startup smoke

It does not describe NSIS/Inno packaging because the repository does not implement that yet.

## Prerequisites

- Conan available in `PATH`
- CMake 3.22+
- Ninja
- Qt 6.6.x runtime/build dependencies
- `curl`
- `python3`
- Docker only if you want PlantUML rendering through `python3 build.py docs`

## Build

Recommended build path:

```bash
python3 build.py all
```

Useful narrower entrypoints:

```bash
python3 build.py server
python3 build.py client
python3 build.py tests
python3 build.py docs
python3 build.py smoke
python3 build.py smoke-gui
```

## Runtime Configuration

### Server environment variables

| Variable | Default | Purpose |
| --- | --- | --- |
| `SAGESTORE_SERVER_ADDRESS` | `127.0.0.1` | HTTP bind address |
| `SAGESTORE_SERVER_PORT` | `8001` | HTTP bind port |
| `SAGESTORE_DB_PATH` | build-time default under `build/_server/db.sl3` | SQLite database file |
| `SAGESTORE_DB_SQL_PATH` | build-time default under `build/_server/scripts/create_db.sql` | bootstrap SQL path |

### Client environment variables

| Variable | Default | Purpose |
| --- | --- | --- |
| `SAGESTORE_SERVER_SCHEME` | `http` | API scheme |
| `SAGESTORE_SERVER_ADDRESS` | `127.0.0.1` | API host |
| `SAGESTORE_SERVER_PORT` | `8001` | API port |
| `SAGESTORE_CLIENT_AUTO_EXIT_MS` | disabled | optional auto-exit timer for smoke/startup runs |

## Launch

### Default local launch

```bash
./build/_server/SageStoreServer
./build/_client/SageStoreClient
```

### Custom local launch

```bash
export SAGESTORE_SERVER_ADDRESS=127.0.0.1
export SAGESTORE_SERVER_PORT=18081
export SAGESTORE_DB_PATH="$PWD/build/_server/local-demo.db"
./build/_server/SageStoreServer
```

Then in a second terminal:

```bash
export SAGESTORE_SERVER_ADDRESS=127.0.0.1
export SAGESTORE_SERVER_PORT=18081
./build/_client/SageStoreClient
```

### WSL GUI launch

Client only:

```bash
scripts/wsl/run_gui_app.sh ./build/_client/SageStoreClient
```

Server + client:

```bash
scripts/wsl/run_fullstack_gui.sh
```

## Smoke Validation

The current canonical smoke path is:

```bash
python3 build.py smoke
```

This command is a validation step, not a build step. It expects the built binaries to already exist.

It starts the built server against an isolated temporary SQLite file, exercises:

- ProductType create
- ProductType list/readback
- Stock create
- Stock update
- Stock delete

The underlying script is:

```bash
scripts/smoke/fullstack_inventory_smoke.sh
```

For a minimal desktop runtime/startup proof in offscreen mode:

```bash
python3 build.py smoke-gui
```

This command also expects the built binaries to already exist.

The underlying script is:

```bash
scripts/smoke/fullstack_gui_startup_smoke.sh
```

## CI / Artifact Reality

- Jenkins now runs `python3 build.py docs`, CMake build/test, the inventory smoke script, and an offscreen GUI startup smoke.
- Successful Jenkins runs archive the built client/server binaries, the bootstrap SQL file, and this deployment runbook.
- No installer bundle or publish stage exists yet.

## Known Limits

- No live automated Qt client against a real server path exists yet.
- ProductType and stock tracking are the current desktop inventory slices.
- The GUI smoke validates startup/runtime only; it does not drive an interactive end-to-end user workflow.
- Archived CI outputs are not a self-contained bundle; the binaries still rely on external Qt/Conan/system shared libraries.
- Packaging and release publishing remain manual.
