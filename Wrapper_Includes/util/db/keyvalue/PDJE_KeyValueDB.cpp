#include "util/db/keyvalue/PDJE_KeyValueDB.hpp"

#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"
#include "util/db/backends/RocksDbBackend.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <cstddef>
#include <memory>
#include <span>

using namespace godot;

namespace godot::pdje_keyvalue_db_internal {

using NativeKeyValueBackend = PDJE_UTIL::db::backends::RocksDbBackend;

struct State {
    NativeKeyValueBackend backend;
    String                path;
    bool                  is_open = false;
};

} // namespace godot::pdje_keyvalue_db_internal

namespace {

using godot::pdje_keyvalue_db_internal::NativeKeyValueBackend;
using godot::pdje_keyvalue_db_internal::State;
using godot::pdje_low_level_util::common::BytesToPackedByteArray;
using godot::pdje_low_level_util::common::KeysToPackedStringArray;
using godot::pdje_low_level_util::common::PackedByteArrayToBytes;
using godot::pdje_low_level_util::common::StatusCodeToGodotCode;
using godot::pdje_low_level_util::common::StatusMessageToGodot;
using godot::pdje_public_util::common::print_method_error;

String
FormatStatusDetail(const PDJE_UTIL::common::Status &status)
{
    const String code    = StatusCodeToGodotCode(status.code);
    const String message = StatusMessageToGodot(status);
    if (message.is_empty()) {
        return code;
    }
    return "[" + code + "] " + message;
}

void
PrintStatusError(const char *method_name, const PDJE_UTIL::common::Status &status)
{
    print_method_error(method_name, FormatStatusDetail(status));
}

bool
ValidatePath(const char *method_name, const String &path)
{
    if (!path.is_empty()) {
        return true;
    }

    print_method_error(method_name, "'path' must not be empty");
    return false;
}

bool
RequireOpen(const char *method_name, const std::unique_ptr<State> &state)
{
    if (state != nullptr && state->is_open) {
        return true;
    }

    print_method_error(method_name, "KeyValue database is not open");
    return false;
}

NativeKeyValueBackend::config_type
MakeConfig(const String &path,
           bool          create_if_missing,
           bool          truncate_if_exists,
           bool          read_only)
{
    return { GpathToCPath(path),
             { create_if_missing, truncate_if_exists, read_only } };
}

void
ResetState(State &state)
{
    state.path    = String();
    state.is_open = false;
}

bool
CloseState(const char *method_name, const std::unique_ptr<State> &state)
{
    if (state == nullptr || !state->is_open) {
        return true;
    }

    auto closed = state->backend.close();
    if (!closed.ok()) {
        PrintStatusError(method_name, closed.status());
        return false;
    }

    ResetState(*state);
    return true;
}

} // namespace

void
PDJE_KeyValueDB::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("Create", "path", "truncate_if_exists"),
                         &PDJE_KeyValueDB::Create,
                         DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Destroy", "path"),
                         &PDJE_KeyValueDB::Destroy);
    ClassDB::bind_method(D_METHOD("Open",
                                  "path",
                                  "create_if_missing",
                                  "truncate_if_exists",
                                  "read_only"),
                         &PDJE_KeyValueDB::Open,
                         DEFVAL(false),
                         DEFVAL(false),
                         DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Close"), &PDJE_KeyValueDB::Close);
    ClassDB::bind_method(D_METHOD("IsOpen"), &PDJE_KeyValueDB::IsOpen);
    ClassDB::bind_method(D_METHOD("GetPath"), &PDJE_KeyValueDB::GetPath);
    ClassDB::bind_method(D_METHOD("Contains", "key"),
                         &PDJE_KeyValueDB::Contains);
    ClassDB::bind_method(D_METHOD("GetText", "key"),
                         &PDJE_KeyValueDB::GetText);
    ClassDB::bind_method(D_METHOD("GetBytes", "key"),
                         &PDJE_KeyValueDB::GetBytes);
    ClassDB::bind_method(D_METHOD("PutText", "key", "value"),
                         &PDJE_KeyValueDB::PutText);
    ClassDB::bind_method(D_METHOD("PutBytes", "key", "value"),
                         &PDJE_KeyValueDB::PutBytes);
    ClassDB::bind_method(D_METHOD("Erase", "key"), &PDJE_KeyValueDB::Erase);
    ClassDB::bind_method(D_METHOD("ListKeys", "prefix"),
                         &PDJE_KeyValueDB::ListKeys,
                         DEFVAL(""));
}

bool
PDJE_KeyValueDB::Create(String path, bool truncate_if_exists)
{
    if (!ValidatePath("PDJE_KeyValueDB.Create", path)) {
        return false;
    }

    auto created =
        NativeKeyValueBackend::create(MakeConfig(path,
                                                 false,
                                                 truncate_if_exists,
                                                 false));
    if (!created.ok()) {
        PrintStatusError("PDJE_KeyValueDB.Create", created.status());
        return false;
    }

    return true;
}

bool
PDJE_KeyValueDB::Destroy(String path)
{
    if (!ValidatePath("PDJE_KeyValueDB.Destroy", path)) {
        return false;
    }

    if (state_ != nullptr && state_->is_open && state_->path == path &&
        !CloseState("PDJE_KeyValueDB.Destroy", state_)) {
        return false;
    }

    auto destroyed =
        NativeKeyValueBackend::destroy(MakeConfig(path, false, false, false));
    if (!destroyed.ok()) {
        PrintStatusError("PDJE_KeyValueDB.Destroy", destroyed.status());
        return false;
    }

    return true;
}

bool
PDJE_KeyValueDB::Open(String path,
                      bool   create_if_missing,
                      bool   truncate_if_exists,
                      bool   read_only)
{
    if (!ValidatePath("PDJE_KeyValueDB.Open", path)) {
        return false;
    }
    if (state_ != nullptr && state_->is_open) {
        print_method_error("PDJE_KeyValueDB.Open",
                           "KeyValue database is already open");
        return false;
    }

    if (state_ == nullptr) {
        state_ = std::make_unique<State>();
    }

    auto opened = state_->backend.open(MakeConfig(path,
                                                  create_if_missing,
                                                  truncate_if_exists,
                                                  read_only));
    if (!opened.ok()) {
        PrintStatusError("PDJE_KeyValueDB.Open", opened.status());
        return false;
    }

    state_->path    = path;
    state_->is_open = true;
    return true;
}

bool
PDJE_KeyValueDB::Close()
{
    if (!RequireOpen("PDJE_KeyValueDB.Close", state_)) {
        return false;
    }
    return CloseState("PDJE_KeyValueDB.Close", state_);
}

bool
PDJE_KeyValueDB::IsOpen() const
{
    return state_ != nullptr && state_->is_open;
}

String
PDJE_KeyValueDB::GetPath() const
{
    return state_ != nullptr ? state_->path : String();
}

bool
PDJE_KeyValueDB::Contains(String key)
{
    if (!RequireOpen("PDJE_KeyValueDB.Contains", state_)) {
        return false;
    }

    auto contains = state_->backend.contains(GStrToCStr(key));
    if (!contains.ok()) {
        PrintStatusError("PDJE_KeyValueDB.Contains", contains.status());
        return false;
    }

    return contains.value();
}

String
PDJE_KeyValueDB::GetText(String key)
{
    if (!RequireOpen("PDJE_KeyValueDB.GetText", state_)) {
        return String();
    }

    auto text = state_->backend.get_text(GStrToCStr(key));
    if (!text.ok()) {
        PrintStatusError("PDJE_KeyValueDB.GetText", text.status());
        return String();
    }

    return CStrToGStr(text.value());
}

PackedByteArray
PDJE_KeyValueDB::GetBytes(String key)
{
    if (!RequireOpen("PDJE_KeyValueDB.GetBytes", state_)) {
        return PackedByteArray();
    }

    auto bytes = state_->backend.get_bytes(GStrToCStr(key));
    if (!bytes.ok()) {
        PrintStatusError("PDJE_KeyValueDB.GetBytes", bytes.status());
        return PackedByteArray();
    }

    return BytesToPackedByteArray(bytes.value());
}

bool
PDJE_KeyValueDB::TryGetBytesSilently(String          key,
                                     PackedByteArray &out_value,
                                     bool            &out_found) const
{
    out_value = PackedByteArray();
    out_found = false;

    if (state_ == nullptr || !state_->is_open) {
        return false;
    }

    auto bytes = state_->backend.get_bytes(GStrToCStr(key));
    if (!bytes.ok()) {
        if (bytes.status().code == PDJE_UTIL::common::StatusCode::not_found) {
            return true;
        }
        return false;
    }

    out_value = BytesToPackedByteArray(bytes.value());
    out_found = true;
    return true;
}

bool
PDJE_KeyValueDB::PutText(String key, String value)
{
    if (!RequireOpen("PDJE_KeyValueDB.PutText", state_)) {
        return false;
    }

    auto stored = state_->backend.put_text(GStrToCStr(key), GStrToCStr(value));
    if (!stored.ok()) {
        PrintStatusError("PDJE_KeyValueDB.PutText", stored.status());
        return false;
    }

    return true;
}

bool
PDJE_KeyValueDB::PutBytes(String key, PackedByteArray value)
{
    if (!RequireOpen("PDJE_KeyValueDB.PutBytes", state_)) {
        return false;
    }

    const auto bytes = PackedByteArrayToBytes(value);
    auto stored = state_->backend.put_bytes(
        GStrToCStr(key),
        std::span<const std::byte>(bytes.data(), bytes.size()));
    if (!stored.ok()) {
        PrintStatusError("PDJE_KeyValueDB.PutBytes", stored.status());
        return false;
    }

    return true;
}

bool
PDJE_KeyValueDB::Erase(String key)
{
    if (!RequireOpen("PDJE_KeyValueDB.Erase", state_)) {
        return false;
    }

    auto erased = state_->backend.erase(GStrToCStr(key));
    if (!erased.ok()) {
        PrintStatusError("PDJE_KeyValueDB.Erase", erased.status());
        return false;
    }

    return true;
}

PackedStringArray
PDJE_KeyValueDB::ListKeys(String prefix)
{
    if (!RequireOpen("PDJE_KeyValueDB.ListKeys", state_)) {
        return PackedStringArray();
    }

    auto keys = state_->backend.list_keys(GStrToCStr(prefix));
    if (!keys.ok()) {
        PrintStatusError("PDJE_KeyValueDB.ListKeys", keys.status());
        return PackedStringArray();
    }

    return KeysToPackedStringArray(keys.value());
}

PDJE_KeyValueDB::PDJE_KeyValueDB() = default;

PDJE_KeyValueDB::~PDJE_KeyValueDB()
{
    (void)CloseState("PDJE_KeyValueDB.~PDJE_KeyValueDB", state_);
}

void
PDJE_KeyValueDB::_ready()
{
}
