# PDJE Godot Wrapper Agent Guide

This repository is a Godot 4 GDExtension wrapper around the upstream
`Project DJ Engine` C++ project. The wrapper target is `PDJE_godot_wrapper`,
and the GDExtension entry symbol is `pdje_wrapper_init`.

## Start Here

1. Read this file.
2. Read [AGENT_DOCS/INDEX.md](AGENT_DOCS/INDEX.md).
3. Read the subsystem document named by the task.
4. If the task requires upstream engine behavior, read
   [Project-DJ-Engine/AGENTS.md](Project-DJ-Engine/AGENTS.md) and the relevant
   upstream `Project-DJ-Engine/AGENT_DOCS/` page.

## Core Rules

- Treat `Project-DJ-Engine/` as an external upstream engine. Do not edit it by
  default from wrapper tasks.
- Prefer wrapper-side integration in `Wrapper_Includes/` before changing
  upstream behavior.
- Keep Godot-facing APIs Godot-friendly: use `Variant`, `String`,
  `Packed*Array`, `RefCounted`, and wrapper-owned conversion code at the
  boundary.
- Register new Godot-exposed C++ classes in
  [Wrapper_Includes/register_types.cpp](Wrapper_Includes/register_types.cpp)
  and include their sources in the wrapper source list in
  [CMakeLists.txt](CMakeLists.txt).

## Default Edit Boundaries

Usually editable:

- `Wrapper_Includes/`
- `cmake/`
- `CMakeLists.txt`
- `CMakePresets.json`
- `PDJE_Wrapper.gdextension`
- `project.godot`
- `AGENTS.md`
- `AGENT_DOCS/`

Edit only with care:

- `BuildInitwithConan.*`
- `conanfile.txt`
- Godot scene/resource files
- sample DB or test resource files

Do not edit by default:

- `Project-DJ-Engine/`
- `build/`
- `conan_cmakes/`
- `.godot/`
- `logs/`
- `sandbox/`
- `rootdb/`
- `kv_cacheDB/`
- vendored or generated dependency trees

## Verification

Documentation-only harness changes do not require a build. For code changes,
use [AGENT_DOCS/VERIFY.md](AGENT_DOCS/VERIFY.md) and confirm before running
commands that may rebuild dependencies, launch Godot, or touch generated output.
