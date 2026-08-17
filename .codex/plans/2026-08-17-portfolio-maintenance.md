## Goal and context

- Audit the public SageStore repository as a portfolio project.
- Establish the implemented Qt client / HTTP server / SQLite scope from code and history.
- Fix only high-confidence correctness, security, portability, documentation, and CI issues.

## Scope and definition of done

- Preserve the existing GPL-3.0 license and repository architecture boundaries.
- Inspect source, tests, configuration, generated artifacts, docs, and automation.
- Build native supported targets and run the full test suite plus practical smoke checks.
- Add a reproducible GitHub Actions build/test gate only if it matches verified local commands.
- Update public metadata only when it is materially inaccurate.
- Commit concerns separately and push normally to `origin/master`.

## Intended files

- `.codex/plans/2026-08-17-portfolio-maintenance.md`: required execution record.
- `.github/workflows/ci.yml`: portable build/test gate, if verified.
- Focused `_client/`, `_server/`, `_common/`, or test files only for confirmed defects.
- `README.md` or focused docs only for confirmed inaccurate or missing operational guidance.

## Approach

- [x] Inspect history, dependencies, architecture, security boundaries, and repository hygiene.
- [x] Establish baseline docs/link checks and native build/test/smoke results.
- [x] Implement and test narrow, high-confidence fixes.
- [x] Add and validate GitHub Actions with pinned actions, minimal permissions, and a timeout.
- [x] Review diff, commits, metadata, push, and verify remote/CI status.

## Risks and mitigations

- Qt/Conan builds are expensive: use the repository's canonical build script and retain logs.
- Runtime services may be unavailable: use the repository's isolated SQLite smoke fixtures.
- Authentication changes can be breaking: prefer tests and documentation unless behavior is clearly unsafe and compatibility is understood.
- Existing docs are extensive: avoid duplicating architecture or overstating maturity.

## Verification

- `python3 scripts/check_docs_links.py`
- `python3 build.py all`
- `python3 build.py smoke`
- `python3 build.py smoke-gui`
- `git diff --check`

## Progress and final review

- 2026-08-17: Started from clean isolated worktree at `origin/master` (`a5ba947`).
- 2026-08-17: Confirmed the implemented MVP spans Qt desktop workflows, HTTP business modules,
  SQLite repositories, a C++ debug CLI, and a legacy Go inspector; existing PlantUML and
  screenshots are current and adequate.
- 2026-08-17: Removed HTTP payload contents from logs and added a password-verifier regression.
- 2026-08-17: Hardened the Go inspector's bind address, upstream handling, registration route,
  and sensitive-data logging; added handler contract coverage.
- 2026-08-17: Fixed system-Boost test linking and GCC 13/system-fmt portability, then passed a
  clean Ubuntu 24.04 native build, all 31 CTest tests, both smoke scripts, docs links, and Go tests.
- 2026-08-17: Upgraded `chi` to 5.2.2 to resolve the open moderate Dependabot alert and
  disabled persisted GitHub checkout credentials; pushed normally and verified the exact-SHA CI.

Final review: the repository remains an honestly labeled, validated ERP MVP rather than a
production release. The existing architecture diagrams and screenshots are adequate. Remaining
production blockers are the documented authentication/authorization/TLS design, schema migration
and backup support, packaging, and incomplete company/storage/document workflows.
