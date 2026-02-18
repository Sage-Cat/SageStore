# SageStore

SageStore is a C++20/Qt desktop ERP project with a client-server architecture.

## Current Project Stage

This branch focuses on stabilizing the foundation for continued development.

| Module | Status | Notes |
| --- | --- | --- |
| Users (login/users/roles) | Implemented | End-to-end client API + server business logic + repository + tests |
| Purchase | Planned | Data schema exists, business/UI flow not implemented end-to-end |
| Sales | Planned | Data schema exists, business/UI flow not implemented end-to-end |
| Inventory | Planned | Data schema exists, business/UI flow not implemented end-to-end |
| Management | Planned | Data schema exists, business/UI flow not implemented end-to-end |
| Analytics | Planned | Not implemented |

Detailed status and execution roadmap: `docs/Implementation_Status.md`.

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
python3 build.py clean
```

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

## VS Code

Workspace tasks are in `.vscode/tasks.json`.

Main task flows:

- `workflow:build+test`
- `workflow:docs`

## Contributing

See `CONTRIBUTING.md`.

## License

GPL v3. See `LICENSE`.
