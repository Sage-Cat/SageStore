#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${SAGESTORE_BUILD_DIR:-${REPO_ROOT}/build}"

require_command() {
    local name="$1"
    if ! command -v "${name}" >/dev/null 2>&1; then
        echo "error: required command not found: ${name}" >&2
        exit 1
    fi
}

require_path() {
    local path="$1"
    local hint="$2"
    if [[ ! -e "${path}" ]]; then
        echo "error: required path not found: ${path}" >&2
        echo "hint: ${hint}" >&2
        exit 1
    fi
}

require_command ctest
require_command bash
require_path "${BUILD_DIR}" "Run 'python3 build.py build' or 'python3 build.py tests' first."

TMP_DIR="$(mktemp -d)"
LOG_DIR="${TMP_DIR}/logs"

cleanup() {
    rm -rf "${TMP_DIR}"
}
trap cleanup EXIT INT TERM

mkdir -p "${LOG_DIR}"

export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}"
export SAGESTORE_FORCE_OFFSCREEN=1
export SAGESTORE_LOG_DIR="${LOG_DIR}"

echo "-> Running offscreen regression-labeled CTest suite"
ctest --test-dir "${BUILD_DIR}" --output-on-failure --verbose -L offscreen_regression -j 4

echo "-> Running inventory smoke with log hygiene gate"
bash "${SCRIPT_DIR}/fullstack_inventory_smoke.sh"

echo "-> Running offscreen GUI startup smoke with log hygiene gate"
bash "${SCRIPT_DIR}/fullstack_gui_startup_smoke.sh"

echo "[OK] Offscreen feature regression passed"
