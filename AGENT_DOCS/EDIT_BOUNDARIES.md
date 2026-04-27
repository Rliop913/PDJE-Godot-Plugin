# Edit Boundaries

Use these boundaries unless the user explicitly asks for a different scope.

## Normally Editable

- `Wrapper_Includes/`
- `cmake/`
- `CMakeLists.txt`
- `CMakePresets.json`
- `PDJE_Wrapper.gdextension`
- `project.godot`
- `AGENTS.md`
- `AGENT_DOCS/`

## Edit With Care

- `BuildInitwithConan.*`: setup scripts can affect local and CI bootstrap.
- `conanfile.txt`: dependency changes can alter the whole build graph.
- Godot `*.tscn`, `*.tres`, `*.res`, and imported resource metadata.
- sample DBs and test assets such as `demodb.db`, `WTC.wav`, and local scene
  resources.

## Do Not Edit By Default

- `Project-DJ-Engine/`: upstream engine. Route through
  [UPSTREAM_PDJE.md](UPSTREAM_PDJE.md).
- `build/`: generated build output.
- `conan_cmakes/`: Conan-generated toolchain and deployed dependency files.
- `.godot/`: Godot-generated editor/import cache.
- `logs/`: runtime logs.
- `sandbox/`: local runtime sandbox data.
- `rootdb/` and `kv_cacheDB/`: local database output.
- vendored/generated dependency trees.

## Dirty Worktree Handling

This repository often contains generated files and local test artifacts. Before
editing, check `git status --short`. Ignore unrelated dirty files and avoid
reverting user or generated changes unless the user explicitly requests it.
