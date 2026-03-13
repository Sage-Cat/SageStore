# SageStore

SageStore is a C++20/Qt desktop ERP project with a client-server architecture.

## Current Project Stage

This branch focuses on stabilizing the foundation for continued development.

| Module | Status | Notes |
| --- | --- | --- |
| Users (login/users/roles) | Implemented | End-to-end client API + server business logic + repository + tests |
| Inventory (ProductType CRUD) | Implemented slice | Shared endpoint + server module/repository + Qt client MVVM/UI + automated tests |
| Inventory (stock tracking) | Implemented slice | Shared endpoint + server module/repository + Qt desktop view/MVVM + HTTP/unit/integration/UI coverage + smoke validation |
| Purchase | Planned | Data schema exists, business/UI flow not implemented end-to-end |
| Sales | Planned | Data schema exists, business/UI flow not implemented end-to-end |
| Management | Planned | Data schema exists, business/UI flow not implemented end-to-end |
| Analytics | Planned | Not implemented |
| Logs | Planned | Schema and placeholder routing exist, but no end-to-end slice |

Detailed status and execution roadmap: `docs/Implementation_Status.md`.
Requirements reconciliation and use-case baseline: `docs/Requirements_Baseline.md`.
Runtime/deployment instructions: `docs/Deployment_Runbook.md`.

## Tech Stack

- Language: C++20
- Client UI: Qt 6.6.x
- Server networking: Boost.Beast/Asio
- Database: SQLite3
- Build: CMake + Conan
- Tests: GoogleTest/GoogleMock + QtTest
- CI: Jenkins
- Docs: Doxygen + PlantUML

## Build

### Recommended (scripted)

```bash
python3 build.py all
```

Other commands:

```bash
python3 build.py client
python3 build.py server
python3 build.py tests
python3 build.py test
python3 build.py smoke
python3 build.py smoke-gui
python3 build.py clean
```

`python3 build.py smoke` and `python3 build.py smoke-gui` are validation commands and expect built binaries to already exist.

### Manual

```bash
conan install . --output-folder=build --build=missing
cmake -S . -B build -G Ninja -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release -DBUILD_CLIENT=ON -DBUILD_SERVER=ON -DBUILD_TESTS=ON
cmake --build build --parallel
```

## Test

```bash
ctest --test-dir build --output-on-failure --verbose
```

## Run

```bash
./build/_server/SageStoreServer
./build/_client/SageStoreClient
```

Both binaries also support simple environment-based configuration:

```bash
export SAGESTORE_SERVER_ADDRESS=127.0.0.1
export SAGESTORE_SERVER_PORT=18081
export SAGESTORE_DB_PATH="$PWD/build/_server/local-demo.db"
./build/_server/SageStoreServer
```

## Run From WSL2 (GUI)

Recommended approach on Windows 11 is running Linux binaries via WSLg (no Windows cross-toolchain needed).

For client-only launch:

```bash
scripts/wsl/run_gui_app.sh ./build/_client/SageStoreClient
```

For server + client launch:

```bash
scripts/wsl/run_fullstack_gui.sh
```

If WSLg is not available, the script falls back to external X server display:

```bash
export SAGESTORE_WSL_X_DISPLAY="sage-pc:0.0"
scripts/wsl/run_gui_app.sh ./build/_client/SageStoreClient
```

## Documentation

Generate PlantUML diagrams (Docker-based):

```bash
scripts/plantuml/render_puml_docker.sh docs png
```

Generate Doxygen API docs:

```bash
doxygen Doxyfile
```

Validate markdown links:

```bash
python3 scripts/check_docs_links.py
```

Canonical docs entrypoint:

```bash
python3 build.py docs
```

## VS Code

Workspace tasks are in `.vscode/tasks.json`.

Task output:

- Colored stage headers and command traces are shown for every task run.

Available team tasks (kept intentionally minimal):

- `SageStore: Clean Rebuild (all targets + docs)` (always rebuilds docs)
- `SageStore: Build (with cache, all targets)`
- `SageStore: Build Documentation`
- `SageStore: Run Tests`
- `SageStore: Run Server (only)`
- `SageStore: Run Client (only)`

## Contributing

See `CONTRIBUTING.md`.

## License

GPL v3. See `LICENSE`.
