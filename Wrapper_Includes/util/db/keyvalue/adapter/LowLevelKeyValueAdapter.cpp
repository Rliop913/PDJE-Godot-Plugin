#include "util/db/keyvalue/adapter/LowLevelKeyValueAdapter.hpp"
#include "util/db/keyvalue/adapter/LowLevelKeyValueAdapterInternal.hpp"

#include "util/common/bridge/LowLevelUtilCommon.hpp"

#include <cstddef>
#include <span>

namespace {

using godot::Dictionary;
using godot::PackedByteArray;
using godot::PackedStringArray;
using godot::String;
using godot::Variant;
using godot::pdje_low_level_util::common::BytesToPackedByteArray;
using godot::pdje_low_level_util::common::DictionaryBoolAt;
using godot::pdje_low_level_util::common::KeysToPackedStringArray;
using godot::pdje_low_level_util::common::MakeStatusResult;
using godot::pdje_low_level_util::common::MakeSuccessResult;
using godot::pdje_low_level_util::common::PackedByteArrayToBytes;
using KeyValueState = godot::pdje_low_level_util_internal::KeyValueState;

struct KeyValueConfigParseResult {
    PDJE_UTIL::db::backends::RocksDbConfig config;
    Dictionary                             normalized;
    PDJE_UTIL::common::Status              status;
};

Dictionary
MakeKeyValuePayload()
{
    Dictionary payload;
    payload["contains"] = false;
    payload["text"]     = String();
    payload["bytes"]    = PackedByteArray();
    payload["keys"]     = PackedStringArray();
    return payload;
}

Dictionary
MakeKeyValueData(const Dictionary &base,
                 const String     &path,
                 const Dictionary &config,
                 const Variant    &key,
                 const Variant    &prefix,
                 const Variant    &text,
                 const Variant    &bytes,
                 const Variant    &contains,
                 const Variant    &keys,
                 bool              is_open)
{
    Dictionary data = base;
    data["path"]    = path;
    data["config"]  = config;
    data["is_open"] = is_open;

    if (key.get_type() != Variant::NIL) {
        data["key"] = key;
    }
    if (prefix.get_type() != Variant::NIL) {
        data["prefix"] = prefix;
    }
    if (text.get_type() != Variant::NIL) {
        data["text"] = text;
    }
    if (bytes.get_type() != Variant::NIL) {
        data["bytes"] = bytes;
    }
    if (contains.get_type() != Variant::NIL) {
        data["contains"] = contains;
    }
    if (keys.get_type() != Variant::NIL) {
        data["keys"] = keys;
    }

    return data;
}

KeyValueConfigParseResult
ParseKeyValueConfig(const Dictionary &input)
{
    KeyValueConfigParseResult parsed;
    parsed.normalized["path"]               = String();
    parsed.normalized["create_if_missing"]  = false;
    parsed.normalized["truncate_if_exists"] = false;
    parsed.normalized["read_only"]          = false;

    if (!input.has("path")) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "KeyValue config requires a non-empty 'path'." };
        return parsed;
    }

    const String path = static_cast<String>(input["path"]);
    if (path.is_empty()) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "KeyValue config requires a non-empty 'path'." };
        return parsed;
    }

    Dictionary open_options;
    if (input.has("open_options")) {
        open_options = static_cast<Dictionary>(input["open_options"]);
    }

    const bool create_if_missing =
        input.has("create_if_missing")
            ? static_cast<bool>(input["create_if_missing"])
            : DictionaryBoolAt(open_options, "create_if_missing", false);
    const bool truncate_if_exists =
        input.has("truncate_if_exists")
            ? static_cast<bool>(input["truncate_if_exists"])
            : DictionaryBoolAt(open_options, "truncate_if_exists", false);
    const bool read_only =
        input.has("read_only")
            ? static_cast<bool>(input["read_only"])
            : DictionaryBoolAt(open_options, "read_only", false);

    parsed.config.path         = GpathToCPath(path);
    parsed.config.open_options = { .create_if_missing  = create_if_missing,
                                   .truncate_if_exists = truncate_if_exists,
                                   .read_only          = read_only };

    parsed.normalized["path"]               = path;
    parsed.normalized["create_if_missing"]  = create_if_missing;
    parsed.normalized["truncate_if_exists"] = truncate_if_exists;
    parsed.normalized["read_only"]          = read_only;
    parsed.status                           = {};
    return parsed;
}

Dictionary
MakeClosedResult(const std::unique_ptr<KeyValueState> &state,
                 const Variant                        &key      = Variant(),
                 const Variant                        &prefix   = Variant(),
                 const Variant                        &text     = Variant(),
                 const Variant                        &bytes    = Variant(),
                 const Variant                        &contains = Variant(),
                 const Variant                        &keys     = Variant())
{
    const PDJE_UTIL::common::Status status{
        PDJE_UTIL::common::StatusCode::closed, "KeyValue database is not open."
    };
    return MakeStatusResult(
        false,
        status,
        MakeKeyValueData(MakeKeyValuePayload(),
                         state != nullptr ? state->path : String(),
                         state != nullptr ? state->config : Dictionary(),
                         key,
                         prefix,
                         text,
                         bytes,
                         contains,
                         keys,
                         false));
}

bool
IsOpen(const std::unique_ptr<KeyValueState> &state)
{
    return state != nullptr && state->is_open;
}

} // namespace

namespace godot::pdje_low_level_util::keyvalue {

Dictionary
Create(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
       const Dictionary                                             &config)
{
    auto parsed = ParseKeyValueConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 String(),
                                                 parsed.normalized,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 IsOpen(state)));
    }

    auto created = pdje_low_level_util_internal::NativeKeyValueBackend::create(
        parsed.config);
    if (!created.ok()) {
        return MakeStatusResult(
            false,
            created.status(),
            MakeKeyValueData(MakeKeyValuePayload(),
                             static_cast<String>(parsed.normalized["path"]),
                             parsed.normalized,
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             IsOpen(state)));
    }

    return MakeSuccessResult(
        MakeKeyValueData(MakeKeyValuePayload(),
                         static_cast<String>(parsed.normalized["path"]),
                         parsed.normalized,
                         Variant(),
                         Variant(),
                         Variant(),
                         Variant(),
                         Variant(),
                         Variant(),
                         IsOpen(state)));
}

Dictionary
Destroy(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
        const Dictionary                                             &config)
{
    auto parsed = ParseKeyValueConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 String(),
                                                 parsed.normalized,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 IsOpen(state)));
    }

    if (IsOpen(state) &&
        state->path == static_cast<String>(parsed.normalized["path"])) {
        auto closed = state->backend.close();
        if (!closed.ok()) {
            return MakeStatusResult(false,
                                    closed.status(),
                                    MakeKeyValueData(MakeKeyValuePayload(),
                                                     state->path,
                                                     state->config,
                                                     Variant(),
                                                     Variant(),
                                                     Variant(),
                                                     Variant(),
                                                     Variant(),
                                                     Variant(),
                                                     true));
        }
        state->is_open = false;
        state->config  = Dictionary();
        state->path    = String();
    }

    auto destroyed =
        pdje_low_level_util_internal::NativeKeyValueBackend::destroy(
            parsed.config);
    if (!destroyed.ok()) {
        return MakeStatusResult(
            false,
            destroyed.status(),
            MakeKeyValueData(MakeKeyValuePayload(),
                             static_cast<String>(parsed.normalized["path"]),
                             parsed.normalized,
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             IsOpen(state)));
    }

    return MakeSuccessResult(
        MakeKeyValueData(MakeKeyValuePayload(),
                         static_cast<String>(parsed.normalized["path"]),
                         parsed.normalized,
                         Variant(),
                         Variant(),
                         Variant(),
                         Variant(),
                         Variant(),
                         Variant(),
                         IsOpen(state)));
}

Dictionary
Open(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
     const Dictionary                                             &config)
{
    auto parsed = ParseKeyValueConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 String(),
                                                 parsed.normalized,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 IsOpen(state)));
    }

    if (IsOpen(state)) {
        const PDJE_UTIL::common::Status status{
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "KeyValue database is already open. Close it before opening "
            "another one."
        };
        return MakeStatusResult(false,
                                status,
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 true));
    }

    if (state == nullptr) {
        state = std::make_unique<KeyValueState>();
    }

    auto opened = state->backend.open(parsed.config);
    if (!opened.ok()) {
        return MakeStatusResult(
            false,
            opened.status(),
            MakeKeyValueData(MakeKeyValuePayload(),
                             static_cast<String>(parsed.normalized["path"]),
                             parsed.normalized,
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             Variant(),
                             false));
    }

    state->config  = parsed.normalized;
    state->path    = static_cast<String>(parsed.normalized["path"]);
    state->is_open = true;

    return MakeSuccessResult(MakeKeyValueData(MakeKeyValuePayload(),
                                              state->path,
                                              state->config,
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              true));
}

Dictionary
Close(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state);
    }

    auto closed = state->backend.close();
    if (!closed.ok()) {
        return MakeStatusResult(false,
                                closed.status(),
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 true));
    }

    Dictionary config = state->config;
    String     path   = state->path;
    state->config     = Dictionary();
    state->path       = String();
    state->is_open    = false;

    return MakeSuccessResult(MakeKeyValueData(MakeKeyValuePayload(),
                                              path,
                                              config,
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              false));
}

Dictionary
Contains(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
         const String                                                 &key)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(
            state, key, Variant(), Variant(), Variant(), false);
    }

    auto contains = state->backend.contains(GStrToCStr(key));
    if (!contains.ok()) {
        return MakeStatusResult(false,
                                contains.status(),
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 key,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 false,
                                                 Variant(),
                                                 true));
    }

    return MakeSuccessResult(MakeKeyValueData(MakeKeyValuePayload(),
                                              state->path,
                                              state->config,
                                              key,
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              contains.value(),
                                              Variant(),
                                              true));
}

Dictionary
GetText(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
        const String                                                 &key)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state, key, Variant(), String());
    }

    auto text = state->backend.get_text(GStrToCStr(key));
    if (!text.ok()) {
        return MakeStatusResult(false,
                                text.status(),
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 key,
                                                 Variant(),
                                                 String(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 true));
    }

    return MakeSuccessResult(MakeKeyValueData(MakeKeyValuePayload(),
                                              state->path,
                                              state->config,
                                              key,
                                              Variant(),
                                              CStrToGStr(text.value()),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              true));
}

Dictionary
GetBytes(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
         const String                                                 &key)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(
            state, key, Variant(), Variant(), PackedByteArray());
    }

    auto bytes = state->backend.get_bytes(GStrToCStr(key));
    if (!bytes.ok()) {
        return MakeStatusResult(false,
                                bytes.status(),
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 key,
                                                 Variant(),
                                                 Variant(),
                                                 PackedByteArray(),
                                                 Variant(),
                                                 Variant(),
                                                 true));
    }

    return MakeSuccessResult(
        MakeKeyValueData(MakeKeyValuePayload(),
                         state->path,
                         state->config,
                         key,
                         Variant(),
                         Variant(),
                         BytesToPackedByteArray(bytes.value()),
                         Variant(),
                         Variant(),
                         true));
}

Dictionary
PutText(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
        const String                                                 &key,
        const String                                                 &value)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state, key, Variant(), value);
    }

    auto stored = state->backend.put_text(GStrToCStr(key), GStrToCStr(value));
    if (!stored.ok()) {
        return MakeStatusResult(false,
                                stored.status(),
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 key,
                                                 Variant(),
                                                 value,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 true));
    }

    return MakeSuccessResult(MakeKeyValueData(MakeKeyValuePayload(),
                                              state->path,
                                              state->config,
                                              key,
                                              Variant(),
                                              value,
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              true));
}

Dictionary
PutBytes(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
         const String                                                 &key,
         const PackedByteArray                                        &value)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state, key, Variant(), Variant(), value);
    }

    const auto bytes  = PackedByteArrayToBytes(value);
    auto       stored = state->backend.put_bytes(
        GStrToCStr(key),
        std::span<const std::byte>(bytes.data(), bytes.size()));
    if (!stored.ok()) {
        return MakeStatusResult(false,
                                stored.status(),
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 key,
                                                 Variant(),
                                                 Variant(),
                                                 value,
                                                 Variant(),
                                                 Variant(),
                                                 true));
    }

    return MakeSuccessResult(MakeKeyValueData(MakeKeyValuePayload(),
                                              state->path,
                                              state->config,
                                              key,
                                              Variant(),
                                              Variant(),
                                              value,
                                              Variant(),
                                              Variant(),
                                              true));
}

Dictionary
Erase(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
      const String                                                 &key)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state, key);
    }

    auto erased = state->backend.erase(GStrToCStr(key));
    if (!erased.ok()) {
        return MakeStatusResult(false,
                                erased.status(),
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 key,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 true));
    }

    return MakeSuccessResult(MakeKeyValueData(MakeKeyValuePayload(),
                                              state->path,
                                              state->config,
                                              key,
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              Variant(),
                                              true));
}

Dictionary
ListKeys(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
         const String                                                 &prefix)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                Variant(),
                                prefix,
                                Variant(),
                                Variant(),
                                Variant(),
                                PackedStringArray());
    }

    auto keys = state->backend.list_keys(GStrToCStr(prefix));
    if (!keys.ok()) {
        return MakeStatusResult(false,
                                keys.status(),
                                MakeKeyValueData(MakeKeyValuePayload(),
                                                 state->path,
                                                 state->config,
                                                 Variant(),
                                                 prefix,
                                                 Variant(),
                                                 Variant(),
                                                 Variant(),
                                                 PackedStringArray(),
                                                 true));
    }

    return MakeSuccessResult(
        MakeKeyValueData(MakeKeyValuePayload(),
                         state->path,
                         state->config,
                         Variant(),
                         prefix,
                         Variant(),
                         Variant(),
                         Variant(),
                         KeysToPackedStringArray(keys.value()),
                         true));
}

void
Shutdown(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state)
{
    if (IsOpen(state)) {
        (void)state->backend.close();
    }
}

} // namespace godot::pdje_low_level_util::keyvalue
