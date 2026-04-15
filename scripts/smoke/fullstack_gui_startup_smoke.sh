#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

SERVER_BIN="${1:-${REPO_ROOT}/build/_server/SageStoreServer}"
CLIENT_BIN="${2:-${REPO_ROOT}/build/_client/SageStoreClient}"
SERVER_SQL_PATH="${SAGESTORE_DB_SQL_PATH:-${REPO_ROOT}/build/_server/scripts/create_db.sql}"
SERVER_ADDRESS="${SAGESTORE_SERVER_ADDRESS:-127.0.0.1}"
SERVER_PORT="${SAGESTORE_SERVER_PORT:-18082}"
SERVER_SCHEME="${SAGESTORE_SERVER_SCHEME:-http}"
CLIENT_AUTO_EXIT_MS="${SAGESTORE_CLIENT_AUTO_EXIT_MS:-1500}"
BASE_URL="${SERVER_SCHEME}://${SERVER_ADDRESS}:${SERVER_PORT}"

require_binary() {
    local path="$1"
    local hint="$2"
    if [[ ! -x "${path}" ]]; then
        echo "error: executable not found: ${path}" >&2
        echo "hint: ${hint}" >&2
        exit 1
    fi
}

require_command() {
    local name="$1"
    if ! command -v "${name}" >/dev/null 2>&1; then
        echo "error: required command not found: ${name}" >&2
        exit 1
    fi
}

require_binary "${SERVER_BIN}" "Run 'python3 build.py server' or 'python3 build.py all' first."
require_binary "${CLIENT_BIN}" "Run 'python3 build.py client' or 'python3 build.py all' first."
require_command curl

if [[ ! -f "${SERVER_SQL_PATH}" ]]; then
    echo "error: SQL bootstrap script not found: ${SERVER_SQL_PATH}" >&2
    exit 1
fi

TMP_DIR="$(mktemp -d)"
SERVER_DB_PATH="${TMP_DIR}/gui-smoke.db"
SERVER_LOG="${TMP_DIR}/server.log"
CLIENT_LOG="${TMP_DIR}/client.log"
LOG_DIR="${TMP_DIR}/logs"

audit_log_file() {
    local label="$1"
    local path="$2"
    if [[ ! -f "${path}" ]]; then
        echo "error: expected log file not found: ${path}" >&2
        exit 1
    fi

    if grep -Eiq '\[(error|critical)\]' "${path}"; then
        echo "error: ${label} contains error-level entries" >&2
        cat "${path}" >&2 || true
        exit 1
    fi
}

audit_log_dir() {
    local dir="$1"
    local found=0
    while IFS= read -r -d '' log_file; do
        found=1
        audit_log_file "$(basename "${log_file}")" "${log_file}"
    done < <(find "${dir}" -type f -name '*.log' -print0)

    if [[ "${found}" -eq 0 ]]; then
        echo "error: no application log files were produced in ${dir}" >&2
        exit 1
    fi
}

cleanup() {
    if [[ -n "${CLIENT_PID:-}" ]] && kill -0 "${CLIENT_PID}" >/dev/null 2>&1; then
        kill "${CLIENT_PID}" >/dev/null 2>&1 || true
        wait "${CLIENT_PID}" 2>/dev/null || true
    fi

    if [[ -n "${SERVER_PID:-}" ]] && kill -0 "${SERVER_PID}" >/dev/null 2>&1; then
        kill "${SERVER_PID}" >/dev/null 2>&1 || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi

    rm -rf "${TMP_DIR}"
}
trap cleanup EXIT INT TERM

echo "-> Starting SageStore server for GUI startup smoke on ${BASE_URL}"
mkdir -p "${LOG_DIR}"
SAGESTORE_LOG_DIR="${LOG_DIR}" \
SAGESTORE_SERVER_ADDRESS="${SERVER_ADDRESS}" \
SAGESTORE_SERVER_PORT="${SERVER_PORT}" \
SAGESTORE_DB_PATH="${SERVER_DB_PATH}" \
SAGESTORE_DB_SQL_PATH="${SERVER_SQL_PATH}" \
"${SERVER_BIN}" >"${SERVER_LOG}" 2>&1 &
SERVER_PID=$!

for _ in $(seq 1 40); do
    if curl --silent --show-error --fail "${BASE_URL}/api/inventory/product-types" >/dev/null 2>&1; then
        break
    fi

    if ! kill -0 "${SERVER_PID}" >/dev/null 2>&1; then
        echo "error: server exited during GUI smoke startup" >&2
        cat "${SERVER_LOG}" >&2 || true
        exit 1
    fi

    sleep 0.25
done

echo "-> Launching SageStore client in offscreen mode"
QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-offscreen}" \
SAGESTORE_LOG_DIR="${LOG_DIR}" \
SAGESTORE_SERVER_SCHEME="${SERVER_SCHEME}" \
SAGESTORE_SERVER_ADDRESS="${SERVER_ADDRESS}" \
SAGESTORE_SERVER_PORT="${SERVER_PORT}" \
SAGESTORE_CLIENT_AUTO_EXIT_MS="${CLIENT_AUTO_EXIT_MS}" \
"${CLIENT_BIN}" >"${CLIENT_LOG}" 2>&1 &
CLIENT_PID=$!

CLIENT_EXIT_CODE=0
for _ in $(seq 1 80); do
    if ! kill -0 "${CLIENT_PID}" >/dev/null 2>&1; then
        wait "${CLIENT_PID}" || CLIENT_EXIT_CODE=$?
        break
    fi

    if ! kill -0 "${SERVER_PID}" >/dev/null 2>&1; then
        echo "error: server exited while client startup smoke was running" >&2
        cat "${SERVER_LOG}" >&2 || true
        exit 1
    fi

    sleep 0.25
done

if [[ -n "${CLIENT_PID:-}" ]] && kill -0 "${CLIENT_PID}" >/dev/null 2>&1; then
    echo "error: client did not exit within expected smoke window" >&2
    cat "${CLIENT_LOG}" >&2 || true
    exit 1
fi

if [[ "${CLIENT_EXIT_CODE}" -ne 0 ]]; then
    echo "error: client exited with code ${CLIENT_EXIT_CODE}" >&2
    cat "${CLIENT_LOG}" >&2 || true
    exit "${CLIENT_EXIT_CODE}"
fi

audit_log_file "server stdout log" "${SERVER_LOG}"
audit_log_file "client stdout log" "${CLIENT_LOG}"
audit_log_dir "${LOG_DIR}"

echo "[OK] Fullstack GUI startup smoke passed"
