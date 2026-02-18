#!/usr/bin/env bash
set -euo pipefail

SOURCE_DIR="${1:-docs}"
OUTPUT_FORMAT="${2:-png}"

if ! command -v docker >/dev/null 2>&1; then
    echo "docker is required but not installed." >&2
    exit 1
fi

if [[ ! -d "${SOURCE_DIR}" ]]; then
    echo "source directory does not exist: ${SOURCE_DIR}" >&2
    exit 1
fi

mapfile -d '' PUML_FILES < <(find "${SOURCE_DIR}" -type f -name '*.puml' -print0)

if [[ ${#PUML_FILES[@]} -eq 0 ]]; then
    echo "No .puml files found under ${SOURCE_DIR}."
    exit 0
fi

for puml_file in "${PUML_FILES[@]}"; do
    docker run --rm \
        -u "$(id -u):$(id -g)" \
        -e HOME=/tmp \
        -e _JAVA_OPTIONS=-Duser.home=/tmp \
        -v "${PWD}:/workspace" \
        -w /workspace \
        plantuml/plantuml \
        "-t${OUTPUT_FORMAT}" \
        "/workspace/${puml_file}"

    rendered_file="${puml_file%.puml}.${OUTPUT_FORMAT}"
    rendered_dir="$(dirname "${rendered_file}")"
    rendered_name="$(basename "${rendered_file}")"

    # Keep docs image links stable: docs/<area>/plantuml/Foo.puml -> docs/<area>/Foo.png
    if [[ "$(basename "${rendered_dir}")" == "plantuml" ]]; then
        target_dir="$(dirname "${rendered_dir}")"
        mv -f "${rendered_file}" "${target_dir}/${rendered_name}"
    fi
done

echo "PlantUML render completed for ${SOURCE_DIR} (format: ${OUTPUT_FORMAT})."
