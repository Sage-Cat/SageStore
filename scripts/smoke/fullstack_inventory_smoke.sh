#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"

SERVER_BIN="${1:-${REPO_ROOT}/build/_server/SageStoreServer}"
SERVER_SQL_PATH="${SAGESTORE_DB_SQL_PATH:-${REPO_ROOT}/build/_server/scripts/create_db.sql}"
SERVER_ADDRESS="${SAGESTORE_SERVER_ADDRESS:-127.0.0.1}"
SERVER_PORT="${SAGESTORE_SERVER_PORT:-18081}"
SERVER_SCHEME="${SAGESTORE_SERVER_SCHEME:-http}"
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
require_command curl
require_command python3

if [[ ! -f "${SERVER_SQL_PATH}" ]]; then
    echo "error: SQL bootstrap script not found: ${SERVER_SQL_PATH}" >&2
    exit 1
fi

TMP_DIR="$(mktemp -d)"
SERVER_DB_PATH="${TMP_DIR}/smoke.db"
SERVER_LOG="${TMP_DIR}/server.log"
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
    if [[ -n "${SERVER_PID:-}" ]] && kill -0 "${SERVER_PID}" >/dev/null 2>&1; then
        kill "${SERVER_PID}" >/dev/null 2>&1 || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi
    rm -rf "${TMP_DIR}"
}
trap cleanup EXIT INT TERM

echo "-> Starting SageStore server for smoke test on ${BASE_URL}"
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
        echo "error: server exited during smoke startup" >&2
        cat "${SERVER_LOG}" >&2 || true
        exit 1
    fi

    sleep 0.25
done

echo "-> Creating product type"
curl --silent --show-error --fail \
    -H 'Content-Type: application/json' \
    -X POST "${BASE_URL}/api/inventory/product-types" \
    -d '{"code":["PT-SMOKE-001"],"barcode":["SMOKE-001"],"name":["Smoke Product"],"description":["Inventory smoke path"],"lastPrice":["15.50"],"unit":["pcs"],"isImported":["1"]}' \
    >/dev/null

PRODUCT_TYPE_DATA="$(curl --silent --show-error --fail "${BASE_URL}/api/inventory/product-types")"
PRODUCT_TYPE_ID="$(
    printf '%s' "${PRODUCT_TYPE_DATA}" | python3 -c '
import json
import sys

payload = json.load(sys.stdin)
codes = payload.get("code", [])
ids = payload.get("id", [])
for index, code in enumerate(codes):
    if code == "PT-SMOKE-001" and index < len(ids):
        print(ids[index])
        break
'
)"

if [[ -z "${PRODUCT_TYPE_ID}" ]]; then
    echo "error: failed to locate created product type id" >&2
    printf '%s\n' "${PRODUCT_TYPE_DATA}" >&2
    exit 1
fi

echo "-> Creating stock record"
curl --silent --show-error --fail \
    -H 'Content-Type: application/json' \
    -X POST "${BASE_URL}/api/inventory/stocks" \
    -d "{\"productTypeId\":[\"${PRODUCT_TYPE_ID}\"],\"quantityAvailable\":[\"23\"],\"employeeId\":[\"1\"]}" \
    >/dev/null

STOCK_DATA="$(curl --silent --show-error --fail "${BASE_URL}/api/inventory/stocks")"
STOCK_ID="$(
    env PRODUCT_TYPE_ID="${PRODUCT_TYPE_ID}" python3 -c '
import json
import os
import sys

product_type_id = os.environ["PRODUCT_TYPE_ID"]
payload = json.loads(sys.stdin.read())
product_type_ids = payload.get("productTypeId", [])
ids = payload.get("id", [])
for index, value in enumerate(product_type_ids):
    if value == product_type_id and index < len(ids):
        print(ids[index])
        break
    ' <<<"${STOCK_DATA}"
)"

if [[ -z "${STOCK_ID}" ]]; then
    echo "error: failed to locate created stock id" >&2
    printf '%s\n' "${STOCK_DATA}" >&2
    exit 1
fi

echo "-> Updating stock record"
curl --silent --show-error --fail \
    -H 'Content-Type: application/json' \
    -X PUT "${BASE_URL}/api/inventory/stocks/${STOCK_ID}" \
    -d "{\"productTypeId\":[\"${PRODUCT_TYPE_ID}\"],\"quantityAvailable\":[\"29\"],\"employeeId\":[\"1\"]}" \
    >/dev/null

UPDATED_STOCK_DATA="$(curl --silent --show-error --fail "${BASE_URL}/api/inventory/stocks")"
UPDATED_QUANTITY="$(
    env STOCK_ID="${STOCK_ID}" python3 -c '
import json
import os
import sys

stock_id = os.environ["STOCK_ID"]
payload = json.loads(sys.stdin.read())
ids = payload.get("id", [])
quantities = payload.get("quantityAvailable", [])
for index, value in enumerate(ids):
    if value == stock_id and index < len(quantities):
        print(quantities[index])
        break
    ' <<<"${UPDATED_STOCK_DATA}"
)"

if [[ "${UPDATED_QUANTITY}" != "29" ]]; then
    echo "error: stock quantity update did not persist" >&2
    printf '%s\n' "${UPDATED_STOCK_DATA}" >&2
    exit 1
fi

echo "-> Deleting stock record"
curl --silent --show-error --fail -X DELETE "${BASE_URL}/api/inventory/stocks/${STOCK_ID}" >/dev/null

FINAL_STOCK_DATA="$(curl --silent --show-error --fail "${BASE_URL}/api/inventory/stocks")"
if env STOCK_ID="${STOCK_ID}" python3 -c '
import json
import os
import sys

stock_id = os.environ["STOCK_ID"]
payload = json.loads(sys.stdin.read())
sys.exit(0 if stock_id not in payload.get("id", []) else 1)
' <<<"${FINAL_STOCK_DATA}"; then
    :
else
    echo "error: stock record still present after delete" >&2
    printf '%s\n' "${FINAL_STOCK_DATA}" >&2
    exit 1
fi

audit_log_file "server stdout log" "${SERVER_LOG}"
audit_log_dir "${LOG_DIR}"

echo "[OK] Fullstack inventory smoke passed"
