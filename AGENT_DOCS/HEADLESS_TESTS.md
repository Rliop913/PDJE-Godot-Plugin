# Headless GDExtension Tests

The standard wrapper runtime test launches the repository as a Godot project
in headless mode and runs
[`tests/headless/util_api_test.gd`](../tests/headless/util_api_test.gd). This
verifies the script-visible API after dynamic library loading; native build or
upstream unit tests alone do not provide that coverage.

## Configure Godot

Provide a Godot 4 executable either as a CMake cache value:

```sh
cmake --preset <host>-relwithdebinfo \
  -DPDJE_GODOT_EXECUTABLE=/absolute/path/to/godot
```

or as an environment variable at test time:

```sh
PDJE_GODOT_EXECUTABLE=/absolute/path/to/godot \
  cmake --build --preset <host>-relwithdebinfo \
  --target pdje_godot_headless_util_test
```

The CMake runner also searches `godot`, `godot4`, `godot.exe`, and
`godot4.exe` on `PATH`.

## Run

```sh
cmake --build --preset <host>-relwithdebinfo \
  --target pdje_godot_headless_util_test
```

The target first builds `PDJE_godot_wrapper`, copies linked runtime libraries
next to it, and then invokes:

```sh
godot --headless --path <repository-root> \
  --script res://tests/headless/util_api_test.gd
```

Do not replace this with a scene-open smoke check. The script must call the
registered APIs and exit nonzero when an assertion fails.

## Current Coverage

The script checks:

- Util class registration in `ClassDB`;
- RocksDB create/open/text/bytes/list/erase/close/destroy;
- SQLite execute/query/parameter/blob/transaction behavior;
- Annoy item CRUD, nearest search, payloads, close, and destroy;
- direct PCM STFT through `PDJE_MIR`;
- Beat This model initialization and PCM inference through `PDJE_AI`.

Test databases are created under `user://pdje_gdextension_headless` and removed
through the same public wrapper APIs.

## Success Contract

Success requires:

- Godot process exit code `0`;
- the marker `PDJE_GODOT_HEADLESS_TESTS_PASSED`;
- no missing GDExtension library or dependency error.

The runner turns any nonzero Godot result into a CMake failure.
