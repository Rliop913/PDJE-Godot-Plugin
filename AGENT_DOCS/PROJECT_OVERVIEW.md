# Project Overview

This repository builds a Godot 4 GDExtension wrapper for `Project DJ Engine`.
The CMake project and shared library target are both named
`PDJE_godot_wrapper`. Godot loads the wrapper through
[../PDJE_Wrapper.gdextension](../PDJE_Wrapper.gdextension), whose entry symbol
is `pdje_wrapper_init`.

## Repository Roles

- `Wrapper_Includes/`: Godot-facing C++ wrapper code.
- `Wrapper_Includes/register_types.cpp`: class registration and extension
  initialization.
- `Project-DJ-Engine/`: upstream engine source included as a subdirectory.
- `cmake/`: wrapper helper modules for Godot, PDJE, and platform setup.
- `project.godot` and `*.tscn`: Godot project and local scenes used for
  loading or manual tests.

## Build Shape

[../CMakeLists.txt](../CMakeLists.txt) includes the upstream engine with:

- `add_subdirectory(${CMAKE_SOURCE_DIR}/Project-DJ-Engine)`
- `PDJE_DYNAMIC=ON`
- `PDJE_TEST=OFF`
- `PDJE_DEV_TEST=OFF`

The wrapper links against the upstream `PDJE` target, Godot C++ bindings, PDJE
include targets, runtime logging, util libraries, and optional input/judge
targets.

## Godot Integration

The extension entrypoint is `pdje_wrapper_init`. It registers an initializer
that runs at `MODULE_INITIALIZATION_LEVEL_SCENE`, then `ClassDB::register_class`
exposes wrapper classes to Godot.

When adding a new Godot-visible type, update all of these surfaces:

- Header and implementation under `Wrapper_Includes/`.
- `ClassDB::register_class<...>()` in
  [../Wrapper_Includes/register_types.cpp](../Wrapper_Includes/register_types.cpp).
- The relevant source list in [../CMakeLists.txt](../CMakeLists.txt).
