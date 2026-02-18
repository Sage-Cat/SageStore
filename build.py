#!/usr/bin/env python3
import argparse
import shutil
import subprocess
import sys
from pathlib import Path

try:
    from colorama import Fore, Style, init

    init(autoreset=True)
except ModuleNotFoundError:
    Fore = Style = None  # type: ignore[assignment]


BUILD_DIR = Path("build")


def _ok_prefix() -> str:
    if Fore and Style:
        return Fore.GREEN + "[OK] " + Style.RESET_ALL
    return "[OK] "


def _fail_prefix() -> str:
    if Fore and Style:
        return Fore.RED + "[FAIL] " + Style.RESET_ALL
    return "[FAIL] "


def run_command(command: list[str], cwd: Path | None = None) -> None:
    cmd_str = " ".join(command)
    print(f"-> {cmd_str}")
    result = subprocess.run(command, cwd=cwd)
    if result.returncode != 0:
        print(f"{_fail_prefix()}Command failed: {cmd_str}", file=sys.stderr)
        sys.exit(result.returncode)
    print(f"{_ok_prefix()}Command succeeded: {cmd_str}")


def clean_build_dir(build_dir: Path) -> None:
    if build_dir.exists():
        print(f"-> Removing {build_dir}")
        shutil.rmtree(build_dir)


def conan_install(build_dir: Path, build_type: str) -> None:
    run_command(
        [
            "conan",
            "install",
            ".",
            f"--output-folder={build_dir}",
            "--build=missing",
            "-s:h",
            f"build_type={build_type}",
            "-s:b",
            f"build_type={build_type}",
        ]
    )


def cmake_configure(build_dir: Path, build_type: str, cmake_options: list[str]) -> None:
    run_command(
        [
            "cmake",
            "-S",
            ".",
            "-B",
            str(build_dir),
            "-G",
            "Ninja",
            f"-DCMAKE_TOOLCHAIN_FILE={build_dir / 'conan_toolchain.cmake'}",
            "-DCMAKE_POLICY_DEFAULT_CMP0091=NEW",
            f"-DCMAKE_BUILD_TYPE={build_type}",
        ]
        + cmake_options
    )


def cmake_build(build_dir: Path) -> None:
    run_command(["cmake", "--build", str(build_dir), "--parallel"])


def ctest_run(build_dir: Path) -> None:
    run_command(["ctest", "--test-dir", str(build_dir), "--output-on-failure", "--verbose"])


def doxygen_generate() -> None:
    run_command(["doxygen", "Doxyfile"])


def docs_links_check() -> None:
    run_command(["python3", "scripts/check_docs_links.py"])


def build_target(target: str, build_type: str, clean: bool, run_tests: bool) -> None:
    target_options: dict[str, list[str]] = {
        "all": ["-DBUILD_CLIENT=ON", "-DBUILD_SERVER=ON", "-DBUILD_TESTS=ON"],
        "client": ["-DBUILD_CLIENT=ON", "-DBUILD_SERVER=OFF", "-DBUILD_TESTS=OFF"],
        "server": ["-DBUILD_CLIENT=OFF", "-DBUILD_SERVER=ON", "-DBUILD_TESTS=OFF"],
        "tests": ["-DBUILD_CLIENT=ON", "-DBUILD_SERVER=ON", "-DBUILD_TESTS=ON"],
    }

    if clean:
        clean_build_dir(BUILD_DIR)
    conan_install(BUILD_DIR, build_type)
    cmake_configure(BUILD_DIR, build_type, target_options[target])
    cmake_build(BUILD_DIR)

    if run_tests or target in {"all", "tests"}:
        ctest_run(BUILD_DIR)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Build and test SageStore.")
    parser.add_argument(
        "command",
        choices=["all", "client", "server", "tests", "configure", "build", "test", "clean", "docs"],
        help="Action to run.",
    )
    parser.add_argument(
        "--build-type",
        default="Release",
        choices=["Debug", "Release"],
        help="CMake build type used for configure/build actions. Default: Release",
    )
    parser.add_argument(
        "--no-clean",
        action="store_true",
        help="Skip cleaning build directory before target commands.",
    )
    parser.add_argument(
        "--run-tests",
        action="store_true",
        help="Run ctest after build for client/server commands.",
    )
    parser.add_argument(
        "--cmake-options",
        nargs=argparse.REMAINDER,
        default=[],
        help="Extra CMake options used only with the configure command.",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()

    if args.command == "clean":
        clean_build_dir(BUILD_DIR)
        return

    if args.command in {"all", "client", "server", "tests"}:
        build_target(args.command, args.build_type, clean=not args.no_clean,
                     run_tests=args.run_tests)
        return

    if args.command == "configure":
        conan_install(BUILD_DIR, args.build_type)
        cmake_configure(BUILD_DIR, args.build_type, args.cmake_options)
        return

    if args.command == "build":
        cmake_build(BUILD_DIR)
        return

    if args.command == "test":
        ctest_run(BUILD_DIR)
        return

    if args.command == "docs":
        docs_links_check()
        doxygen_generate()
        return


if __name__ == "__main__":
    main()
