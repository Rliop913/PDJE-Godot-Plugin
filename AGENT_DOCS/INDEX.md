# Agent Docs Index

Use these docs to orient wrapper work before editing. They document the wrapper
project only; upstream engine details stay in `Project-DJ-Engine/AGENT_DOCS/`.

## Fast Path

1. [PROJECT_OVERVIEW.md](PROJECT_OVERVIEW.md) for repository purpose and build
   shape.
2. [EDIT_BOUNDARIES.md](EDIT_BOUNDARIES.md) before changing files.
3. [PROBE_MAP.md](PROBE_MAP.md) to find the first files and symbols for a task.
4. [WRAPPING_STRATEGY.md](WRAPPING_STRATEGY.md) for the current
   code-pattern-based wrapper strategy.
5. [VERIFY.md](VERIFY.md) before build, Godot load, or runtime checks.
6. [HEADLESS_TESTS.md](HEADLESS_TESTS.md) for executable GDExtension API
   coverage.

## Reference Pages

- [UPSTREAM_PDJE.md](UPSTREAM_PDJE.md): how to route upstream engine questions.
- [WRAPPING_STRATEGY.md](WRAPPING_STRATEGY.md): actual wrapper-side facade,
  handle, data-carrier, DB, MIR, input, and judge patterns.
- [KNOWN_GAPS.md](KNOWN_GAPS.md): current harness limitations and follow-ups.
- [HEADLESS_TESTS.md](HEADLESS_TESTS.md): Godot headless test entrypoint,
  coverage, and failure contract.

## Main Local Files

- [../CMakeLists.txt](../CMakeLists.txt): wrapper target, upstream inclusion,
  source lists, and conditional input/judge build.
- [../CMakePresets.json](../CMakePresets.json): release and relwithdebinfo
  configure/build presets.
- [../PDJE_Wrapper.gdextension](../PDJE_Wrapper.gdextension): GDExtension entry
  symbol and platform library paths.
- [../Wrapper_Includes/register_types.cpp](../Wrapper_Includes/register_types.cpp):
  Godot class registration surface.
- [../Wrapper_Includes/](../Wrapper_Includes/): wrapper source root.
- [../Project-DJ-Engine/AGENTS.md](../Project-DJ-Engine/AGENTS.md): upstream
  engine agent entrypoint.
