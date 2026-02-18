#!/usr/bin/env python3
from __future__ import annotations

import re
import sys
from pathlib import Path

MARKDOWN_LINK_PATTERN = re.compile(r"\[[^\]]+\]\(([^)]+)\)")


def is_ignored_link(link_target: str) -> bool:
    return (
        link_target.startswith("http://")
        or link_target.startswith("https://")
        or link_target.startswith("mailto:")
        or link_target.startswith("#")
    )


def split_link_and_anchor(link_target: str) -> str:
    return link_target.split("#", 1)[0].strip()


def collect_markdown_files(repo_root: Path) -> list[Path]:
    files = [repo_root / "README.md", repo_root / "CONTRIBUTING.md"]
    files.extend((repo_root / "docs").rglob("*.md"))
    return [file for file in files if file.exists()]


def validate_file_links(markdown_file: Path) -> list[str]:
    missing_links: list[str] = []
    content = markdown_file.read_text(encoding="utf-8")

    for match in MARKDOWN_LINK_PATTERN.finditer(content):
        raw_target = match.group(1).strip()
        if is_ignored_link(raw_target):
            continue

        normalized_target = split_link_and_anchor(raw_target)
        if not normalized_target:
            continue

        target_path = (markdown_file.parent / normalized_target).resolve()
        if not target_path.exists():
            missing_links.append(f"{markdown_file}: missing link target `{raw_target}`")

    return missing_links


def main() -> int:
    repo_root = Path(__file__).resolve().parent.parent
    markdown_files = collect_markdown_files(repo_root)

    missing_links: list[str] = []
    for markdown_file in markdown_files:
        missing_links.extend(validate_file_links(markdown_file))

    if missing_links:
        print("Found broken documentation links:")
        for item in missing_links:
            print(f"- {item}")
        return 1

    print(f"Link check passed for {len(markdown_files)} markdown files.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
