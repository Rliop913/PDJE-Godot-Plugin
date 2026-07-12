# Verification

Documentation-only harness changes do not require a CMake or Godot build. For
code changes, choose the smallest verification path that covers the touched
surface.

## Static Checks

- Check Markdown links after editing docs.
- Use `rg` to confirm instructions do not conflict with the upstream boundary:
  `rg "Project-DJ-Engine|upstream|do not edit|edit" AGENTS.md AGENT_DOCS`
- Use `rg` to confirm new wrapper class names appear in headers,
  implementations, registration, and CMake source lists.

## CMake Presets

Inspect available presets before configuring:

```sh
cmake --list-presets
```

Typical configure/build flow after Conan setup exists:

```sh
cmake --preset linux-release
cmake --build --preset linux-release
```

Use the host-appropriate preset from [../CMakePresets.json](../CMakePresets.json)
instead of assuming Linux.

## Conan And Generated Files

Conan setup may write or rewrite `conan_cmakes/`, build trees, and dependency
deployment output. Ask the user before running bootstrap commands that fetch or
regenerate dependencies.

## Godot Loading

To verify Godot integration, confirm that:

- [../PDJE_Wrapper.gdextension](../PDJE_Wrapper.gdextension) points at the built
  `PDJE_godot_wrapper` library for the host and build type.
- The entry symbol remains `pdje_wrapper_init`.
- Godot can open [../project.godot](../project.godot) and load the extension.

The repeatable runtime route is the headless API target documented in
[HEADLESS_TESTS.md](HEADLESS_TESTS.md). It loads the built extension and calls
the registered Util APIs from GDScript. Use manual GUI scenes only for behavior
that requires interactive rendering or input.

Typical host flow after configuring `PDJE_GODOT_EXECUTABLE`:

```sh
cmake --build --preset <host>-relwithdebinfo \
  --target pdje_godot_headless_util_test
```

The target depends on `PDJE_godot_wrapper`, so it rebuilds the extension before
launching Godot headless. A zero exit status and
`PDJE_GODOT_HEADLESS_TESTS_PASSED` are both required.

## Input/Judge Availability

When testing input or judge wrappers, confirm the platform and
`PDJE_DEVELOP_INPUT` state first. Apple builds disable this path by default;
other platforms enable it and define `PDJE_GODOT_ENABLE_INPUT_WRAPPER`.
