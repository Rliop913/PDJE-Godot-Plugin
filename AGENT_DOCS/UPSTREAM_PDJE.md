# Upstream PDJE

`Project-DJ-Engine/` is the upstream engine embedded in this wrapper repository.
Treat it as an external project with its own agent harness.

## Where To Read

- [../Project-DJ-Engine/AGENTS.md](../Project-DJ-Engine/AGENTS.md): upstream
  agent entrypoint.
- [../Project-DJ-Engine/AGENT_DOCS/INDEX.md](../Project-DJ-Engine/AGENT_DOCS/INDEX.md):
  upstream documentation index.
- [../Project-DJ-Engine/AGENT_DOCS/ARCHITECTURE.md](../Project-DJ-Engine/AGENT_DOCS/ARCHITECTURE.md):
  high-level engine architecture.
- [../Project-DJ-Engine/AGENT_DOCS/INVARIANTS.md](../Project-DJ-Engine/AGENT_DOCS/INVARIANTS.md):
  invariants to check before assuming a behavior change is safe.
- [../Project-DJ-Engine/AGENT_DOCS/VERIFY.md](../Project-DJ-Engine/AGENT_DOCS/VERIFY.md):
  upstream verification paths.

## Wrapper Rules

- Do not directly modify upstream files for ordinary wrapper work.
- If upstream behavior appears wrong, first confirm whether the wrapper can
  adapt through conversion, lifecycle management, or a narrower Godot-facing API.
- If upstream changes are unavoidable, read the upstream docs first and keep the
  wrapper and upstream change scopes separate.
- Do not duplicate upstream architecture docs in this repository. Link to them
  and document only wrapper-specific consequences here.

## Public Entrypoints From This Repository

The wrapper consumes upstream through CMake targets and headers exposed by
`Project-DJ-Engine`. The primary local integration points are:

- `add_subdirectory(${CMAKE_SOURCE_DIR}/Project-DJ-Engine)` in
  [../CMakeLists.txt](../CMakeLists.txt).
- `PDJE_INCLUDE_ROOT` pointing at the upstream directory.
- Link target `PDJE` and upstream include/helper targets in the wrapper link
  list.
