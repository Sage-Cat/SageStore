#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

DRY_RUN=false
if [[ "${1:-}" == "--dry-run" ]]; then
    DRY_RUN=true
    shift
fi

SERVER_BIN="${1:-${REPO_ROOT}/build/_server/SageStoreServer}"
CLIENT_BIN="${2:-${REPO_ROOT}/build/_client/SageStoreClient}"

if [[ ! -x "${SERVER_BIN}" ]]; then
    echo "error: server is not executable or not found: ${SERVER_BIN}" >&2
    exit 1
fi

if [[ ! -x "${CLIENT_BIN}" ]]; then
    echo "error: client is not executable or not found: ${CLIENT_BIN}" >&2
    exit 1
fi

SERVER_START_WAIT_SEC="${SAGESTORE_SERVER_WAIT_SECONDS:-1}"

if [[ "${DRY_RUN}" == "true" ]]; then
    echo "[sagestore-wsl] dry-run server=${SERVER_BIN} client=${CLIENT_BIN}"
    "${SCRIPT_DIR}/run_gui_app.sh" --dry-run "${CLIENT_BIN}"
    exit 0
fi

echo "[sagestore-wsl] starting server: ${SERVER_BIN}"
"${SERVER_BIN}" &
SERVER_PID=$!

cleanup() {
    if kill -0 "${SERVER_PID}" >/dev/null 2>&1; then
        echo "[sagestore-wsl] stopping server (pid=${SERVER_PID})"
        kill "${SERVER_PID}" >/dev/null 2>&1 || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi
}
trap cleanup EXIT INT TERM

sleep "${SERVER_START_WAIT_SEC}"
if ! kill -0 "${SERVER_PID}" >/dev/null 2>&1; then
    echo "error: server process exited during startup (pid=${SERVER_PID})" >&2
    exit 1
fi

echo "[sagestore-wsl] launching client: ${CLIENT_BIN}"
"${SCRIPT_DIR}/run_gui_app.sh" "${CLIENT_BIN}"
