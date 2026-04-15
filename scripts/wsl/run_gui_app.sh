#!/usr/bin/env bash
set -euo pipefail

usage() {
    cat <<'USAGE'
Usage: scripts/wsl/run_gui_app.sh [--dry-run] <app_path> [app_args...]

WSL behavior:
1. Select GUI mode from `SAGESTORE_WSL_GUI_MODE` (`auto`, `wslg`, `xserver`).
2. In `auto` mode, prefer an external X server when one is reachable on
   `SAGESTORE_WSL_X_DISPLAY`, `127.0.0.1:0.0`, or `<windows-host-ip>:0.0`.
3. If no external X server is reachable, fallback to the existing WSLg
   environment (WAYLAND_DISPLAY and/or DISPLAY).
4. By default, an existing QT_QPA_PLATFORM=offscreen is ignored for interactive
   launches. Set SAGESTORE_FORCE_OFFSCREEN=1 to explicitly keep headless mode.
5. By default on WSLg, Wayland is preferred when available; if unavailable,
   launcher falls back to xcb automatically. Set
   SAGESTORE_QPA_PLATFORM=xcb to force xcb.
6. SAGESTORE_CLIENT_AUTO_EXIT_MS is ignored for interactive launches unless
   QT_QPA_PLATFORM=offscreen is requested.
USAGE
}

detect_wsl_x_display() {
    if [[ -n "${SAGESTORE_WSL_X_DISPLAY:-}" ]]; then
        echo "${SAGESTORE_WSL_X_DISPLAY}"
        return 0
    fi

    echo "127.0.0.1:0.0"
    return 0
}

tcp_port_reachable() {
    local host="$1"
    local port="$2"

    if command -v nc >/dev/null 2>&1; then
        nc -z "${host}" "${port}" >/dev/null 2>&1
        return $?
    fi

    if command -v timeout >/dev/null 2>&1; then
        timeout 1 bash -lc "exec 3<>/dev/tcp/${host}/${port}" >/dev/null 2>&1
        return $?
    fi

    bash -lc "exec 3<>/dev/tcp/${host}/${port}" >/dev/null 2>&1
}

detect_reachable_external_x_display() {
    if [[ -n "${SAGESTORE_WSL_X_DISPLAY:-}" ]]; then
        echo "${SAGESTORE_WSL_X_DISPLAY}"
        return 0
    fi

    if tcp_port_reachable "127.0.0.1" "6000"; then
        echo "127.0.0.1:0.0"
        return 0
    fi

    if [[ -r /etc/resolv.conf ]]; then
        local host_ip
        host_ip="$(awk '/^nameserver[[:space:]]+/ {print $2; exit}' /etc/resolv.conf)"
        if [[ -n "${host_ip}" ]] && tcp_port_reachable "${host_ip}" "6000"; then
            echo "${host_ip}:0.0"
            return 0
        fi
    fi

    return 1
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

qt_platform_plugin_usable() {
    local platform_name="$1"
    local pattern="*plugins/platforms/libq${platform_name}*.so"
    local plugin_file=""
    local plugin_dir=""
    local -a plugin_dirs=()

    if command -v qtpaths >/dev/null 2>&1; then
        while IFS=':' read -r plugin_dir; do
            plugin_dirs+=("${plugin_dir}")
        done < <(qtpaths --plugin-dirs 2>/dev/null || true)
    fi

    if [[ "${#plugin_dirs[@]}" -eq 0 ]]; then
        plugin_dirs=("${HOME}/.conan2")
    fi

    for search_dir in "${plugin_dirs[@]}"; do
        if [[ -z "${search_dir}" ]]; then
            continue
        fi

        plugin_file="$(find "${search_dir}" -type f -path "${pattern}" -print -quit 2>/dev/null || true)"
        if [[ -n "${plugin_file}" ]]; then
            return 0
        fi
    done

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

FORCE_OFFSCREEN=${SAGESTORE_FORCE_OFFSCREEN:-0}
WSL_GUI_MODE="${SAGESTORE_WSL_GUI_MODE:-auto}"

if is_wsl_environment; then
    if [[ "${FORCE_OFFSCREEN}" != "1" && "${QT_QPA_PLATFORM:-}" == "offscreen" ]]; then
        echo "[sagestore-wsl] overriding QT_QPA_PLATFORM=offscreen for interactive launch; set SAGESTORE_FORCE_OFFSCREEN=1 to keep headless mode"
        unset QT_QPA_PLATFORM
    fi

    if [[ "${QT_QPA_PLATFORM:-}" == "wayland" ]] && ! qt_platform_plugin_usable "wayland"; then
        echo "[sagestore-wsl] requested QT_QPA_PLATFORM=wayland but plugin not found; falling back to xcb"
        export QT_QPA_PLATFORM="xcb"
    fi

    case "${WSL_GUI_MODE}" in
        auto)
            if EXTERNAL_X_DISPLAY="$(detect_reachable_external_x_display)"; then
                MODE="wsl2-external-xserver"
                export DISPLAY="${EXTERNAL_X_DISPLAY}"
                unset WAYLAND_DISPLAY
                echo "[sagestore-wsl] using external X server at DISPLAY=${DISPLAY}"
            elif [[ -n "${WAYLAND_DISPLAY:-}" || -n "${DISPLAY:-}" ]]; then
                MODE="wslg"
                echo "[sagestore-wsl] external X server is not reachable from WSL; using WSLg"
            else
                MODE="wsl2-external-xserver"
                export DISPLAY="$(detect_wsl_x_display)"
                unset WAYLAND_DISPLAY
            fi
            ;;
        xserver)
            MODE="wsl2-external-xserver"
            export DISPLAY="$(detect_wsl_x_display)"
            unset WAYLAND_DISPLAY
            echo "[sagestore-wsl] forcing external X server at DISPLAY=${DISPLAY}"
            ;;
        wslg)
            MODE="wslg"
            ;;
        *)
            echo "error: unsupported SAGESTORE_WSL_GUI_MODE='${WSL_GUI_MODE}' (expected auto, wslg, xserver)" >&2
            exit 1
            ;;
    esac

    if [[ -z "${QT_QPA_PLATFORM:-}" ]]; then
        case "${SAGESTORE_QPA_PLATFORM:-}" in
            wayland)
                if qt_platform_plugin_usable "wayland"; then
                    export QT_QPA_PLATFORM="wayland"
                else
                    echo "[sagestore-wsl] wayland platform plugin not found; falling back to xcb"
                    if qt_platform_plugin_usable "xcb"; then
                        export QT_QPA_PLATFORM="xcb"
                    else
                        echo "[sagestore-wsl] xcb plugin not usable; falling back to offscreen"
                        export QT_QPA_PLATFORM="offscreen"
                    fi
                fi
                ;;
            xcb)
                if qt_platform_plugin_usable "xcb"; then
                    export QT_QPA_PLATFORM="xcb"
                else
                    echo "[sagestore-wsl] xcb plugin not usable; falling back to offscreen"
                    export QT_QPA_PLATFORM="offscreen"
                fi
                ;;
            *)
                if [[ "${MODE}" == "wslg" && -n "${WAYLAND_DISPLAY:-}" ]]; then
                    if qt_platform_plugin_usable "wayland"; then
                        export QT_QPA_PLATFORM="wayland"
                    else
                        echo "[sagestore-wsl] wayland platform plugin not found; falling back to xcb"
                        if qt_platform_plugin_usable "xcb"; then
                            export QT_QPA_PLATFORM="xcb"
                        else
                            echo "[sagestore-wsl] xcb plugin not usable; falling back to offscreen"
                            export QT_QPA_PLATFORM="offscreen"
                        fi
                    fi
                else
                    if qt_platform_plugin_usable "xcb"; then
                        export QT_QPA_PLATFORM="xcb"
                    else
                        echo "[sagestore-wsl] xcb plugin not usable; falling back to offscreen"
                        export QT_QPA_PLATFORM="offscreen"
                    fi
                fi
                ;;
        esac
    fi
    export QT_X11_NO_MITSHM="${QT_X11_NO_MITSHM:-1}"

    if [[ "${MODE}" == "wsl2-external-xserver" ]]; then
        export LIBGL_ALWAYS_INDIRECT="${LIBGL_ALWAYS_INDIRECT:-1}"
    fi
fi

if [[ "${QT_QPA_PLATFORM:-}" != "offscreen" && -n "${SAGESTORE_CLIENT_AUTO_EXIT_MS:-}" ]]; then
    if [[ "${SAGESTORE_FORCE_CLIENT_AUTO_EXIT_MS:-0}" != "1" ]]; then
        echo "[sagestore-wsl] ignoring SAGESTORE_CLIENT_AUTO_EXIT_MS for interactive launch (${SAGESTORE_CLIENT_AUTO_EXIT_MS} ms)"
        echo "[sagestore-wsl] set SAGESTORE_FORCE_CLIENT_AUTO_EXIT_MS=1 to keep it"
        unset SAGESTORE_CLIENT_AUTO_EXIT_MS
    else
        echo "[sagestore-wsl] honoring SAGESTORE_CLIENT_AUTO_EXIT_MS (${SAGESTORE_CLIENT_AUTO_EXIT_MS} ms) due explicit force flag"
    fi
fi

echo "[sagestore-wsl] mode=${MODE} DISPLAY=${DISPLAY:-<unset>} WAYLAND_DISPLAY=${WAYLAND_DISPLAY:-<unset>} QT_QPA_PLATFORM=${QT_QPA_PLATFORM:-<unset>}"
echo "[sagestore-wsl] command=${APP_PATH} $*"

if [[ "${DRY_RUN}" == "true" ]]; then
    exit 0
fi

exec "${APP_PATH}" "$@"
