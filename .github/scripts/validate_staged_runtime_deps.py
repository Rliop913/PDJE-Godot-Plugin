#!/usr/bin/env python3
"""Validate staged runtime dependencies for PDJE GitHub Actions artifacts."""

from __future__ import annotations

import os
import re
import shutil
import subprocess
import sys
from pathlib import Path


PROJECT_PREFIXES_POSIX = ("libPDJE", "libonnxruntime", "libhwy")
PROJECT_PREFIXES_WINDOWS = ("onnxruntime", "zlib", "hwy", "libhwy")
LINUX_NEEDED_RE = re.compile(r"\(NEEDED\).*Shared library:\s*\[([^\]]+)\]")
LINUX_RPATH_RE = re.compile(r"\((?:RUNPATH|RPATH)\).*Library r(?:un)?path:\s*\[([^\]]*)\]")


def fail(message: str) -> None:
    print(f"error: {message}", file=sys.stderr)
    sys.exit(1)


def run(command: list[str], env: dict[str, str] | None = None) -> str:
    completed = subprocess.run(
        command,
        check=False,
        env=env,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    output = completed.stdout + completed.stderr
    if completed.returncode != 0:
        print(output)
        fail(f"{command[0]} failed for {command[-1]}")
    return output


def basename_posix(value: str) -> str:
    return value.replace("\\", "/").rsplit("/", 1)[-1]


def staged_names(staged_dir: Path, case_insensitive: bool = False) -> set[str]:
    names = {entry.name for entry in staged_dir.iterdir() if not entry.is_dir()}
    if case_insensitive:
        return {name.lower() for name in names}
    return names


def require_staged(
    dep_name: str,
    target: Path,
    names: set[str],
    failures: list[str],
    *,
    case_insensitive: bool = False,
) -> None:
    key = dep_name.lower() if case_insensitive else dep_name
    if key not in names:
        failures.append(f"{target.name} references {dep_name}, but it is not in staged_bins")


def is_zlib_posix_dep(dep_name: str) -> bool:
    return (
        dep_name == "libz.so"
        or dep_name.startswith("libz.so.")
        or dep_name == "libz.dylib"
        or re.fullmatch(r"libz\..*\.dylib", dep_name) is not None
    )


def is_project_posix_dep(dep_name: str) -> bool:
    return dep_name.startswith(PROJECT_PREFIXES_POSIX) or is_zlib_posix_dep(dep_name)


def linux_deps(output: str) -> tuple[list[str], list[str]]:
    deps: list[str] = []
    missing: list[str] = []
    for raw_line in output.splitlines():
        line = raw_line.strip()
        if not line:
            continue

        if "=>" in line:
            dep = line.split("=>", 1)[0].strip()
            if "not found" in line:
                missing.append(dep)
            deps.append(dep)
            continue

        token = line.split(maxsplit=1)[0]
        if ".so" in token:
            deps.append(token)

    return [basename_posix(dep) for dep in deps], [basename_posix(dep) for dep in missing]


def linux_needed_deps(output: str) -> list[str]:
    deps: list[str] = []
    for raw_line in output.splitlines():
        match = LINUX_NEEDED_RE.search(raw_line)
        if match:
            deps.append(basename_posix(match.group(1)))
    return deps


def linux_rpaths(output: str) -> list[str]:
    rpaths: list[str] = []
    for raw_line in output.splitlines():
        match = LINUX_RPATH_RE.search(raw_line)
        if match:
            rpaths.extend(entry for entry in match.group(1).split(":") if entry)
    return rpaths


def validate_linux(staged_dir: Path) -> list[str]:
    if shutil.which("readelf") is None:
        return ["readelf is required to validate Linux runtime dependencies"]

    names = staged_names(staged_dir)
    targets = sorted(
        entry
        for entry in staged_dir.iterdir()
        if not entry.is_dir() and entry.name.startswith("libPDJE") and ".so" in entry.name
    )
    if not targets:
        return ["no staged libPDJE*.so targets were found"]

    failures: list[str] = []
    ldd_env = os.environ.copy()
    existing_library_path = ldd_env.get("LD_LIBRARY_PATH")
    staged_library_path = str(staged_dir.resolve())
    ldd_env["LD_LIBRARY_PATH"] = (
        f"{staged_library_path}:{existing_library_path}" if existing_library_path else staged_library_path
    )
    for target in targets:
        ldd_output = run(["ldd", str(target)], env=ldd_env)
        _, missing = linux_deps(ldd_output)
        for dep in missing:
            failures.append(f"{target.name} has unresolved dependency {dep}")
        needed_output = run(["readelf", "-d", str(target)])
        needed_deps = linux_needed_deps(needed_output)
        rpaths = linux_rpaths(needed_output)
        for dep in needed_deps:
            if is_project_posix_dep(dep):
                require_staged(dep, target, names, failures)
        if any(is_project_posix_dep(dep) for dep in needed_deps) and "$ORIGIN" not in rpaths:
            failures.append(
                f"{target.name} references staged runtime libraries, but RUNPATH/RPATH lacks $ORIGIN"
            )

    return failures


def macos_deps(output: str) -> list[str]:
    deps: list[str] = []
    for raw_line in output.splitlines()[1:]:
        line = raw_line.strip()
        if not line:
            continue
        deps.append(basename_posix(line.split(maxsplit=1)[0]))
    return deps


def validate_macos(staged_dir: Path) -> list[str]:
    if shutil.which("otool") is None:
        return ["otool is required to validate macOS runtime dependencies"]

    names = staged_names(staged_dir)
    targets = sorted(
        entry
        for entry in staged_dir.iterdir()
        if not entry.is_dir() and entry.name.startswith("libPDJE") and entry.name.endswith(".dylib")
    )
    if not targets:
        return ["no staged libPDJE*.dylib targets were found"]

    failures: list[str] = []
    for target in targets:
        output = run(["otool", "-L", str(target)])
        for dep in macos_deps(output):
            if is_project_posix_dep(dep):
                require_staged(dep, target, names, failures)

    return failures


def windows_deps(output: str) -> list[str]:
    deps: list[str] = []
    for raw_line in output.splitlines():
        line = raw_line.strip()
        if not line:
            continue

        dll_name_match = re.search(r"(?i)^DLL Name:\s*(\S+\.dll)\b", line)
        if dll_name_match:
            deps.append(basename_posix(dll_name_match.group(1)))
            continue

        indented_match = re.search(r"(?i)^([A-Za-z0-9_.+-]+\.dll)\b", line)
        if indented_match:
            deps.append(basename_posix(indented_match.group(1)))

    return deps


def validate_windows(staged_dir: Path) -> list[str]:
    tool = shutil.which("dumpbin")
    command_prefix: list[str]
    if tool is not None:
        command_prefix = [tool, "/DEPENDENTS"]
    else:
        tool = shutil.which("llvm-objdump")
        if tool is None:
            return ["dumpbin or llvm-objdump is required to validate Windows runtime dependencies"]
        command_prefix = [tool, "-p"]

    names = staged_names(staged_dir, case_insensitive=True)
    targets = sorted(
        entry
        for entry in staged_dir.iterdir()
        if not entry.is_dir() and entry.name.lower().endswith(".dll") and "pdje" in entry.name.lower()
    )
    if not targets:
        return ["no staged PDJE*.dll targets were found"]

    failures: list[str] = []
    for target in targets:
        output = run(command_prefix + [str(target)])
        for dep in windows_deps(output):
            dep_lower = dep.lower()
            is_project_dep = "pdje" in dep_lower or dep_lower.startswith(PROJECT_PREFIXES_WINDOWS)
            if is_project_dep:
                require_staged(dep, target, names, failures, case_insensitive=True)

    return failures


def main() -> None:
    if len(sys.argv) != 2:
        fail("usage: validate_staged_runtime_deps.py <staged_bins>")

    staged_dir = Path(sys.argv[1])
    if not staged_dir.is_dir():
        fail(f"{staged_dir} is not a directory")

    runner_os = os.environ.get("RUNNER_OS")
    if runner_os == "Linux" or (runner_os is None and sys.platform.startswith("linux")):
        failures = validate_linux(staged_dir)
    elif runner_os == "macOS" or (runner_os is None and sys.platform == "darwin"):
        failures = validate_macos(staged_dir)
    elif runner_os == "Windows" or (runner_os is None and sys.platform.startswith("win")):
        failures = validate_windows(staged_dir)
    else:
        fail(f"unsupported runner OS: {runner_os or sys.platform}")

    if failures:
        for failure in failures:
            print(f"error: {failure}", file=sys.stderr)
        sys.exit(1)

    print("staged runtime dependency validation passed")


if __name__ == "__main__":
    main()
