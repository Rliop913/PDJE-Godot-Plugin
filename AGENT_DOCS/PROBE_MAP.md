# Probe Map

Use this map to start investigation quickly. Prefer `rg` for symbol and path
searches.

## Extension Loading

Read first:

- [../PDJE_Wrapper.gdextension](../PDJE_Wrapper.gdextension)
- [../Wrapper_Includes/register_types.cpp](../Wrapper_Includes/register_types.cpp)
- [../Wrapper_Includes/register_types.h](../Wrapper_Includes/register_types.h)

Search:

- `rg "pdje_wrapper_init|entry_symbol|MODULE_INITIALIZATION_LEVEL_SCENE"`
- `rg "ClassDB::register_class"`

## Build And Linkage

Read first:

- [../CMakeLists.txt](../CMakeLists.txt)
- [../CMakePresets.json](../CMakePresets.json)
- [../cmake/](../cmake/)

Search:

- `rg "PDJE_godot_wrapper|WrapperSrcs|UtilBundleSrcs"`
- `rg "PDJE_DYNAMIC|PDJE_TEST|PDJE_DEV_TEST|PDJE_DEVELOP_INPUT"`
- `rg "PDJE_GODOT_ENABLE_INPUT_WRAPPER"`

## Core, Player, And Editor Wrappers

Read first:

- [../Wrapper_Includes/core/PDJE_Core_Wrapper.hpp](../Wrapper_Includes/core/PDJE_Core_Wrapper.hpp)
- [../Wrapper_Includes/core/PDJE_Core_Wrapper.cpp](../Wrapper_Includes/core/PDJE_Core_Wrapper.cpp)
- [../Wrapper_Includes/core/Player/](../Wrapper_Includes/core/Player/)
- [../Wrapper_Includes/core/Editor/](../Wrapper_Includes/core/Editor/)

Search:

- `rg "PDJE_Wrapper|PlayerWrapper|EditorWrapper|MusPanelWrapper|FXWrapper"`
- `rg "_bind_methods|ClassDB::bind_method"`
- `rg "std::optional<PDJE>|std::shared_ptr<audioPlayer>|MusicControlPanel|FXControlPanel" Wrapper_Includes/core`

## Wrapping Pattern Routes

Core facade:

- [../Wrapper_Includes/core/PDJE_Core_Wrapper.hpp](../Wrapper_Includes/core/PDJE_Core_Wrapper.hpp)
- [../Wrapper_Includes/core/PDJE_Core_Wrapper.cpp](../Wrapper_Includes/core/PDJE_Core_Wrapper.cpp)
- Search: `rg "InitEngine|GetPlayer|GetEditor|PullOutCoreLine|GetNoteObjects" Wrapper_Includes/core`

Handle wrappers:

- [../Wrapper_Includes/core/Player/PlayerWrapper.hpp](../Wrapper_Includes/core/Player/PlayerWrapper.hpp)
- [../Wrapper_Includes/core/Editor/EditorWrapper.hpp](../Wrapper_Includes/core/Editor/EditorWrapper.hpp)
- [../Wrapper_Includes/core/Player/MusPanelWrapper.hpp](../Wrapper_Includes/core/Player/MusPanelWrapper.hpp)
- [../Wrapper_Includes/core/Player/FXWrapper.hpp](../Wrapper_Includes/core/Player/FXWrapper.hpp)
- Search: `rg "void[[:space:]]+Init|shared_ptr|\\*.*= nullptr" Wrapper_Includes/core`

Data carriers:

- [../Wrapper_Includes/core/Editor/WrappedEditorArgs.hpp](../Wrapper_Includes/core/Editor/WrappedEditorArgs.hpp)
- [../Wrapper_Includes/util/db/nearest/PDJE_VectorTypes.hpp](../Wrapper_Includes/util/db/nearest/PDJE_VectorTypes.hpp)
- [../Wrapper_Includes/util/db/relational/PDJE_RelationalTypes.hpp](../Wrapper_Includes/util/db/relational/PDJE_RelationalTypes.hpp)
- [../Wrapper_Includes/util/MIR/PDJE_StftResult.hpp](../Wrapper_Includes/util/MIR/PDJE_StftResult.hpp)
- Search: `rg "PDJE_EDITOR_ARG|PDJE_VectorItem|PDJE_VectorHit|PDJE_RelationalRow|PDJE_RelationalExecResult|PDJE_StftResult" Wrapper_Includes`

Boundary conversions and errors:

- [../Wrapper_Includes/global/pdje_util_common.hpp](../Wrapper_Includes/global/pdje_util_common.hpp)
- [../Wrapper_Includes/util/common/bridge/LowLevelUtilCommon.hpp](../Wrapper_Includes/util/common/bridge/LowLevelUtilCommon.hpp)
- [../Wrapper_Includes/util/common/bridge/PublicUtilBridge.hpp](../Wrapper_Includes/util/common/bridge/PublicUtilBridge.hpp)
- Search: `rg "GStrToCStr|CStrToGStr|GpathToCPath|print_method_error|call_or_error|value_or_error" Wrapper_Includes`

## Data Lines

Read first:

- [../Wrapper_Includes/global/DataLine/CoreLine.hpp](../Wrapper_Includes/global/DataLine/CoreLine.hpp)
- [../Wrapper_Includes/global/DataLine/InputLine.hpp](../Wrapper_Includes/global/DataLine/InputLine.hpp)

Search:

- `rg "CoreLine|InputLine|DATA_LINE|DataLine"`
- `rg "PullOutCoreLine|PullOutRawCoreLine|PullOutRawDataLine|emit_input_signal"`

## MIR Utilities

Read first:

- [../Wrapper_Includes/util/MIR/PDJE_MIR.hpp](../Wrapper_Includes/util/MIR/PDJE_MIR.hpp)
- [../Wrapper_Includes/util/MIR/PDJE_MIR_Bindings.cpp](../Wrapper_Includes/util/MIR/PDJE_MIR_Bindings.cpp)
- [../Wrapper_Includes/util/MIR/PDJE_MIR_STFT.cpp](../Wrapper_Includes/util/MIR/PDJE_MIR_STFT.cpp)
- [../Wrapper_Includes/util/MIR/PDJE_MIR_Waveform.cpp](../Wrapper_Includes/util/MIR/PDJE_MIR_Waveform.cpp)

Search:

- `rg "PDJE_MIR|PDJE_StftResult|LowLevelWaveformAdapter|STFT|waveform"`
- `rg "SoundToWaveform|SoundToRGBWaveform|STFT_MUSIC|STFT_PCM_DATA|TryGetBytesSilently|Build.*CacheKey" Wrapper_Includes/util/MIR Wrapper_Includes/util/db/keyvalue`

## AI Utilities

Read first:

- [../Wrapper_Includes/util/AI/PDJE_AI.hpp](../Wrapper_Includes/util/AI/PDJE_AI.hpp)
- [../Wrapper_Includes/util/AI/beat_this/PDJE_BeatThisDetector.hpp](../Wrapper_Includes/util/AI/beat_this/PDJE_BeatThisDetector.hpp)
- [../Wrapper_Includes/util/AI/beat_this/PDJE_BeatThisResult.hpp](../Wrapper_Includes/util/AI/beat_this/PDJE_BeatThisResult.hpp)

Search:

- `rg "PDJE_AI|PDJE_BeatThisDetector|PDJE_BeatThisResult" Wrapper_Includes CMakeLists.txt`
- `rg "BeatThisDetector|BeatDetectionResult" Project-DJ-Engine/include/util/ai`

The wrapper v1 AI surface exposes Beat This only. Generic ONNX Runtime session
and tensor APIs stay internal to upstream `PDJE_UTIL::ai`.

## Database Utilities

Read first:

- [../Wrapper_Includes/util/db/keyvalue/PDJE_KeyValueDB.hpp](../Wrapper_Includes/util/db/keyvalue/PDJE_KeyValueDB.hpp)
- [../Wrapper_Includes/util/db/nearest/PDJE_VectorDB.hpp](../Wrapper_Includes/util/db/nearest/PDJE_VectorDB.hpp)
- [../Wrapper_Includes/util/db/relational/PDJE_RelationalDB.hpp](../Wrapper_Includes/util/db/relational/PDJE_RelationalDB.hpp)

Search:

- `rg "PDJE_KeyValueDB|PDJE_VectorDB|PDJE_RelationalDB"`
- `rg "RocksDB|sqlite|nearest|VectorHit|RelationalRow"`
- `rg "struct State|NativeKeyValueBackend|NativeNearestIndex|NativeRelationalDatabase|RequireOpen|PrintStatusError" Wrapper_Includes/util/db`

## Input And Judge

Read first:

- [../Wrapper_Includes/input/PDJE_Input_Wrapper.hpp](../Wrapper_Includes/input/PDJE_Input_Wrapper.hpp)
- [../Wrapper_Includes/judge/PDJE_Judge_Wrapper.hpp](../Wrapper_Includes/judge/PDJE_Judge_Wrapper.hpp)
- [../Wrapper_Includes/global/DataLine/InputLine.hpp](../Wrapper_Includes/global/DataLine/InputLine.hpp)

Search:

- `rg "PDJE_Input_Module|PDJE_Judge_Module|PDJE_GODOT_ENABLE_INPUT_WRAPPER"`
- `rg "PDJE_DEVELOP_INPUT|PDJE_INPUT_LINK_LIB|PDJE_MODULE_INPUT"`
- `rg "InitializeInputLine|emit_input_signal|AddDataLines|SetCustomEvents|call_deferred" Wrapper_Includes/input Wrapper_Includes/judge Wrapper_Includes/global/DataLine`

## Headless Godot Tests

Read first:

- [HEADLESS_TESTS.md](HEADLESS_TESTS.md)
- [../tests/headless/util_api_test.gd](../tests/headless/util_api_test.gd)
- [../cmake/RunGodotHeadlessUtilTests.cmake](../cmake/RunGodotHeadlessUtilTests.cmake)

Search:

- `rg "pdje_godot_headless_util_test|PDJE_GODOT_EXECUTABLE"`
- `rg "_test_|PDJE_GODOT_HEADLESS_TESTS" tests/headless`

## Upstream Behavior

Read first:

- [UPSTREAM_PDJE.md](UPSTREAM_PDJE.md)
- [../Project-DJ-Engine/AGENTS.md](../Project-DJ-Engine/AGENTS.md)
- [../Project-DJ-Engine/AGENT_DOCS/INDEX.md](../Project-DJ-Engine/AGENT_DOCS/INDEX.md)

Search from repository root:

- `rg "symbol_or_type" Project-DJ-Engine/include Project-DJ-Engine/cmakes/src`
