# Wrapping Strategy

Keep the wrapper thin. Its job is to present upstream PDJE capabilities in a
Godot-friendly API while hiding upstream C++ details, ownership, and conversion
rules from scripts and scenes.

This page documents the current wrapper-side strategy. It is not an upstream
engine design document and it does not promise API stability beyond the code in
this repository.

## Godot Class Registration

New Godot-visible classes need three updates:

- Add the wrapper header and implementation under `Wrapper_Includes/`.
- Register the class in
  [../Wrapper_Includes/register_types.cpp](../Wrapper_Includes/register_types.cpp).
- Add the implementation file to the relevant source list in
  [../CMakeLists.txt](../CMakeLists.txt).

Use existing wrapper classes as local style references before inventing a new
layout.

## Core Facade

`PDJE_Wrapper` is the central Godot facade. It derives from `Node`, owns engine
lifetime through `std::optional<PDJE> engine`, and initializes that upstream
instance from `InitEngine(String DBPath)`.

Keep engine setup, player/editor creation, search calls, PCM extraction, note
object callbacks, and core data-line extraction behind this facade. Scripts
should receive Godot-friendly values such as `bool`, `String`, `Array`,
`Dictionary`, `PackedFloat32Array`, or `Ref<>` handles instead of upstream C++
objects.

Callers are expected to initialize the engine before asking for player, editor,
MIR, or judge behavior that dereferences `engine`.

## Handle Wrappers

`PlayerWrapper`, `EditorWrapper`, `MusPanelWrapper`, and `FXWrapper` are
`RefCounted` handles around upstream runtime objects.

- `PlayerWrapper` stores `std::shared_ptr<audioPlayer>` plus a `PDJE *`.
- `EditorWrapper` stores `std::shared_ptr<editorObject>` plus a `PDJE *`.
- `MusPanelWrapper` stores a raw `MusicControlPanel *` plus a `PDJE *`.
- `FXWrapper` stores a raw `FXControlPanel *`.

Use this pattern when a Godot script needs to operate on an upstream object
owned elsewhere. The wrapper should expose methods in Godot types and keep the
upstream pointer/shared pointer private.

## Data Carriers

Use small `RefCounted` data objects when Godot needs to pass structured values
around without owning an upstream subsystem.

Current examples:

- `PDJE_EDITOR_ARG` carries one of the editor argument variants through
  optional upstream edit structs.
- `PDJE_VectorItem` and `PDJE_VectorHit` carry vector DB item/search result
  fields.
- `PDJE_RelationalRow` and `PDJE_RelationalExecResult` carry SQL row and exec
  result fields.
- `PDJE_StftResult` carries STFT real/imaginary packed float arrays.

Prefer these carrier types over leaking upstream structs into method signatures.

## Data-Line Bridges

`CoreLine` and `InputLine` bridge upstream data lines into Godot.

`PDJE_Wrapper::PullOutCoreLine()` wraps the upstream `PDJE_CORE_DATA_LINE` in a
`Ref<CoreLine>`. `CoreLine` stores the data-line struct and exposes polling
methods such as engine time, cursors, and pre-rendered frames as packed arrays
or integers.

`PDJE_Input_Module::InitializeInputLine()` gives an `InputLine` the upstream
`PDJE_INPUT_DATA_LINE`. `InputLine` is a `Node` because it converts polled input
and MIDI logs into Godot signals through `emit_input_signal()`.

## Database Utility Facades

`PDJE_KeyValueDB`, `PDJE_VectorDB`, and `PDJE_RelationalDB` are independent
utility facades. They do not require `PDJE_Wrapper`; each owns a wrapper-side
`State` object and a native backend:

- `PDJE_KeyValueDB` wraps the RocksDB backend and exposes text/bytes/key
  operations.
- `PDJE_VectorDB` wraps the Annoy nearest-neighbor index and exposes vector
  item CRUD plus search.
- `PDJE_RelationalDB` wraps the SQLite relational database and exposes execute,
  query, and transaction methods.

Keep backend config, open/close state, status conversion, and native value
conversion inside the wrapper implementation. The Godot API should continue to
return `bool`, `String`, `Packed*Array`, `Array`, or `RefCounted` result
objects.

## MIR Facade

`PDJE_MIR` is a calculation and cache facade. It receives a `PDJE_Wrapper *` to
search music and obtain PCM from the upstream engine. It may also receive a
`PDJE_KeyValueDB *` cache; cache use is optional and guarded by the key-value DB
being open.

Current public results are:

- `Array` from `SoundToWaveform()` and `SoundToRGBWaveform()` for encoded image
  chunks.
- `PackedColorArray` from `STFT_MUSIC()`.
- `TypedArray<Ref<PDJE_StftResult>>` from `STFT_PCM_DATA()`.

Keep the STFT engine pointer, waveform/STFT cache keys, mutexes, and PCM
splitting details inside the MIR implementation.

## Input And Judge Signal Bridges

Input and judge wrapper classes are conditional. In
[../CMakeLists.txt](../CMakeLists.txt), `PDJE_DEVELOP_INPUT` is set to `OFF` on
Apple platforms and `ON` elsewhere. When enabled, the wrapper:

- Adds `PDJE_Input_Wrapper.cpp`, `PDJE_Judge_Wrapper.cpp`, and `InputLine.cpp`.
- Links input and judge include/module targets.
- Defines `PDJE_GODOT_ENABLE_INPUT_WRAPPER`.

[../Wrapper_Includes/register_types.cpp](../Wrapper_Includes/register_types.cpp)
only includes and registers input/judge classes under
`PDJE_GODOT_ENABLE_INPUT_WRAPPER`. Do not assume those Godot classes exist on
all platforms.

`PDJE_Input_Module` owns the upstream `PDJE_Input`, configures devices with
Godot `Array`/`Dictionary` values, exposes backend/device state, and pulls a raw
input data line for `InputLine`.

`InputLine` converts polling into signals. Its `emit_input_signal()` drains the
upstream input/MIDI buffers and emits keyboard, mouse, and MIDI signals with
Godot scalar values.

`PDJE_Judge_Module` connects the core and input data lines through
`AddDataLines(PDJE_Input_Module *, PDJE_Wrapper *)`. It converts judge callbacks
for miss/use/custom mouse parse events into Godot signals and exposes rule,
device, note, start, and stop methods.

## Boundary Types

Prefer Godot-native boundary types:

- `Variant`
- `String`
- `PackedByteArray`, `PackedFloat32Array`, and related packed arrays
- `Array` and `Dictionary` where Godot scripting ergonomics matter
- `RefCounted` for shared Godot object lifetimes

Keep conversions to upstream structs, enums, spans, buffers, paths, and handles
inside wrapper implementation files.

## Conversion Helpers

Use the existing boundary helpers before adding new conversion code:

- `GStrToCStr`, `CStrToGStr`, and `GpathToCPath` in
  [../Wrapper_Includes/global/pdje_util_common.hpp](../Wrapper_Includes/global/pdje_util_common.hpp)
  for strings and project paths.
- [../Wrapper_Includes/util/common/bridge/LowLevelUtilCommon.hpp](../Wrapper_Includes/util/common/bridge/LowLevelUtilCommon.hpp)
  for low-level status, byte array, key array, and numeric `Variant`
  conversions.
- [../Wrapper_Includes/util/common/bridge/PublicUtilBridge.hpp](../Wrapper_Includes/util/common/bridge/PublicUtilBridge.hpp)
  for public wrapper helpers such as method error reporting, packed array
  parsing, cache DB checks, and cache source keys.

For binary, vector, SQL, and cache values, prefer these helpers over ad hoc
string or byte manipulation.

## Failure And Callback Conventions

The current wrapper does not expose C++ exceptions to Godot. It reports failure
with the narrowest Godot-facing fallback for the method:

- `false` for boolean operations.
- Empty `Array`, `Dictionary`, `Packed*Array`, or `String` values for value
  queries.
- Null or default `Ref<>` values for missing result objects.
- `print_error` or `print_method_error` for diagnostic detail.

Callback-based upstream events should become Godot signals. When a runtime
callback emits a signal, prefer the existing
`call_deferred("emit_signal", ...)` pattern so the signal crosses into Godot on
the main thread path used by the current wrappers.

## Subsystem Direction

- Core/player/editor wrappers live under `Wrapper_Includes/core/`.
- Shared data-line wrappers live under `Wrapper_Includes/global/DataLine/`.
- MIR wrappers live under `Wrapper_Includes/util/MIR/`.
- Key-value, relational, and nearest/vector DB wrappers live under
  `Wrapper_Includes/util/db/`.
- Input and judge wrappers live under `Wrapper_Includes/input/` and
  `Wrapper_Includes/judge/`.

## Upstream Changes

If a feature seems to require upstream behavior changes, first read
[UPSTREAM_PDJE.md](UPSTREAM_PDJE.md). Check upstream invariants and decide
whether the wrapper can solve the Godot integration problem without editing
`Project-DJ-Engine/`.
