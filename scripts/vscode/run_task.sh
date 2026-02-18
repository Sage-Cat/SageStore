#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${REPO_ROOT}/build"
BUILD_TYPE="${SAGESTORE_BUILD_TYPE:-Release}"

if [[ -t 1 ]]; then
    C_RESET='\033[0m'
    C_BOLD='\033[1m'
    C_BLUE='\033[1;34m'
    C_GREEN='\033[1;32m'
    C_YELLOW='\033[1;33m'
    C_DIM='\033[2m'
else
    C_RESET=''
    C_BOLD=''
    C_BLUE=''
    C_GREEN=''
    C_YELLOW=''
    C_DIM=''
fi

stage_index=0
stage() {
    stage_index=$((stage_index + 1))
    printf "%b\n" "${C_BOLD}${C_BLUE}==> Stage ${stage_index}: $*${C_RESET}"
}

info() {
    printf "%b\n" "${C_YELLOW}$*${C_RESET}"
}

success() {
    printf "%b\n" "${C_GREEN}$*${C_RESET}"
}

run_cmd() {
    printf "%b\n" "${C_DIM}$*${C_RESET}"
    "$@"
}

require_executable() {
    local binary="$1"
    local hint="$2"
    if [[ ! -x "${binary}" ]]; then
        printf "%b\n" "error: executable not found: ${binary}" >&2
        printf "%b\n" "hint: ${hint}" >&2
        exit 1
    fi
}

configure_all_targets() {
    run_cmd cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE="${BUILD_DIR}/conan_toolchain.cmake" \
        -DCMAKE_POLICY_DEFAULT_CMP0091=NEW \
        -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
        -DBUILD_CLIENT=ON -DBUILD_SERVER=ON -DBUILD_TESTS=ON
}

build_with_cache() {
    stage "Install Conan dependencies (${BUILD_TYPE})"
    run_cmd conan install "${REPO_ROOT}" --output-folder="${BUILD_DIR}" --build=missing -s "build_type=${BUILD_TYPE}"

    stage "Configure CMake (all targets)"
    configure_all_targets

    stage "Build all configured targets"
    run_cmd cmake --build "${BUILD_DIR}" --parallel

    success "Build completed."
}

build_docs() {
    stage "Validate markdown links"
    run_cmd python3 "${REPO_ROOT}/scripts/check_docs_links.py"

    stage "Render PlantUML diagrams"
    run_cmd "${REPO_ROOT}/scripts/plantuml/render_puml_docker.sh" docs png

    stage "Generate Doxygen documentation"
    run_cmd doxygen "${REPO_ROOT}/Doxyfile"

    success "Documentation rebuild completed."
}

clean_rebuild() {
    stage "Clean build directory"
    run_cmd cmake -E rm -rf "${BUILD_DIR}"

    build_with_cache

    stage "Rebuild project documentation"
    build_docs

    success "Clean rebuild completed (code + docs)."
}

run_tests() {
    stage "Run CTest test suites"
    run_cmd ctest --test-dir "${BUILD_DIR}" --output-on-failure --verbose
    success "All tests completed."
}

run_server() {
    local server_bin="${BUILD_DIR}/_server/SageStoreServer"
    require_executable "${server_bin}" "Run 'SageStore: Build (with cache, all targets)' first."

    stage "Run server executable"
    exec "${server_bin}"
}

run_client() {
    local client_bin="${BUILD_DIR}/_client/SageStoreClient"
    local gui_runner="${REPO_ROOT}/scripts/wsl/run_gui_app.sh"

    require_executable "${client_bin}" "Run 'SageStore: Build (with cache, all targets)' first."
    require_executable "${gui_runner}" "Ensure scripts/wsl/run_gui_app.sh exists and is executable."

    stage "Run client executable (GUI)"
    exec "${gui_runner}" "${client_bin}"
}

usage() {
    cat <<'USAGE'
Usage: scripts/vscode/run_task.sh <command>

Commands:
  clean-rebuild   Clean build + full rebuild + documentation rebuild
  build-cache     Incremental build using existing cache (configure + build)
  docs            Rebuild documentation artifacts
  test            Run all CTest tests
  run-server      Run server executable only
  run-client      Run client executable only
USAGE
}

main() {
    local command="${1:-}"

    case "${command}" in
        clean-rebuild)
            clean_rebuild
            ;;
        build-cache)
            build_with_cache
            ;;
        docs)
            build_docs
            ;;
        test)
            run_tests
            ;;
        run-server)
            run_server
            ;;
        run-client)
            run_client
            ;;
        -h|--help|help|"")
            usage
            ;;
        *)
            printf "%b\n" "error: unknown command '${command}'" >&2
            usage
            exit 1
            ;;
    esac
}

main "$@"
