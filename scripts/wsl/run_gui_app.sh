#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'USAGE'
Usage: scripts/wsl/run_gui_app.sh [--dry-run] <app_path> [app_args...]

WSL behavior:
1. Prefer existing WSLg environment (WAYLAND_DISPLAY and/or DISPLAY).
2. If no GUI env is detected, fallback to DISPLAY from SAGESTORE_WSL_X_DISPLAY,
   then to default "sage-pc:0.0".
USAGE
}

is_wsl_environment() {
    if [[ -f /proc/sys/kernel/osrelease ]] && grep -qi "microsoft" /proc/sys/kernel/osrelease; then
        return 0
    fi

    if [[ -f /proc/version ]] && grep -qi "microsoft" /proc/version; then
        return 0
    fi

    return 1
}

DRY_RUN=false
if [[ "${1:-}" == "--dry-run" ]]; then
    DRY_RUN=true
    shift
fi

if [[ "${1:-}" == "-h" || "${1:-}" == "--help" || "$#" -lt 1 ]]; then
    usage
    exit 0
fi

APP_PATH="$1"
shift

if [[ ! -x "${APP_PATH}" ]]; then
    echo "error: app is not executable or not found: ${APP_PATH}" >&2
    exit 1
fi

MODE="native-linux"

if is_wsl_environment; then
    if [[ -n "${WAYLAND_DISPLAY:-}" || -n "${DISPLAY:-}" ]]; then
        MODE="wslg"
    else
        MODE="wsl2-external-xserver"
        export DISPLAY="${SAGESTORE_WSL_X_DISPLAY:-sage-pc:0.0}"
    fi

    # Qt Widgets app: prefer XCB for consistent behavior across WSLg and external X servers.
    export QT_QPA_PLATFORM="${QT_QPA_PLATFORM:-xcb}"
    export QT_X11_NO_MITSHM="${QT_X11_NO_MITSHM:-1}"

    if [[ "${MODE}" == "wsl2-external-xserver" ]]; then
        export LIBGL_ALWAYS_INDIRECT="${LIBGL_ALWAYS_INDIRECT:-1}"
    fi
fi

echo "[sagestore-wsl] mode=${MODE} DISPLAY=${DISPLAY:-<unset>} WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-<unset>} QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-<unset>}"
echo "[sagestore-wsl] command=${APP_PATH} $*"

if [[ "${DRY_RUN}" == "true" ]]; then
    exit 0
fi

exec "${APP_PATH}" "$@"
