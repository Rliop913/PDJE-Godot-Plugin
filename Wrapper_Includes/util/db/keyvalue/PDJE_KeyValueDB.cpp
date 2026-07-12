#include "util/db/keyvalue/PDJE_KeyValueDB.hpp"

#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"
#include "util/db/backends/RocksDbBackend.hpp"
#include "util/db/keyvalue/Database.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <cstddef>
#include <memory>
#include <span>
#include <utility>

using namespace godot;

namespace godot::pdje_keyvalue_db_internal {

using NativeKeyValueDatabase = PDJE_UTIL::db::keyvalue::KeyValueDatabase<
    PDJE_UTIL::db::backends::RocksDbBackend>;

struct State {
    NativeKeyValueDatabase database;
    String                 path;
};

} // namespace godot::pdje_keyvalue_db_internal

namespace {

using godot::pdje_keyvalue_db_internal::NativeKeyValueDatabase;
using godot::pdje_keyvalue_db_internal::State;
using godot::pdje_low_level_util::common::BytesToPackedByteArray;
using godot::pdje_low_level_util::common::KeysToPackedStringArray;
using godot::pdje_low_level_util::common::PackedByteArrayToBytes;
using godot::pdje_public_util::common::call_or_error;
using godot::pdje_public_util::common::print_method_error;
using godot::pdje_public_util::common::value_or_error;

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
    if (state != nullptr && state->database.is_open) {
        return true;
    }

    print_method_error(method_name, "KeyValue database is not open");
    return false;
}

NativeKeyValueDatabase::config_type
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
    state.path = String();
}

bool
CloseState(const char *method_name, const std::unique_ptr<State> &state)
{
    if (state == nullptr || !state->database.is_open) {
        return true;
    }

    if (!call_or_error(method_name, [&]() { state->database.close(); })) {
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

    const auto config = MakeConfig(path, false, false, false);
    return call_or_error("PDJE_KeyValueDB.Create", [&]() {
        if (truncate_if_exists) {
            NativeKeyValueDatabase::destroy(config);
        }
        NativeKeyValueDatabase::create(config);
    });
}

bool
PDJE_KeyValueDB::Destroy(String path)
{
    if (!ValidatePath("PDJE_KeyValueDB.Destroy", path)) {
        return false;
    }

    if (state_ != nullptr && state_->database.is_open &&
        state_->path == path &&
        !CloseState("PDJE_KeyValueDB.Destroy", state_)) {
        return false;
    }

    return call_or_error("PDJE_KeyValueDB.Destroy", [&]() {
        NativeKeyValueDatabase::destroy(
            MakeConfig(path, false, false, false));
    });
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
    if (state_ != nullptr && state_->database.is_open) {
        print_method_error("PDJE_KeyValueDB.Open",
                           "KeyValue database is already open");
        return false;
    }

    if (state_ == nullptr) {
        state_ = std::make_unique<State>();
    }

    auto opened = value_or_error("PDJE_KeyValueDB.Open", [&]() {
        return NativeKeyValueDatabase::open(MakeConfig(path,
                                                       create_if_missing,
                                                       truncate_if_exists,
                                                       read_only));
    });
    if (!opened.has_value()) {
        return false;
    }

    state_->database = std::move(*opened);
    state_->path     = path;
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
    return state_ != nullptr && state_->database.is_open;
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

    auto contains = value_or_error("PDJE_KeyValueDB.Contains", [&]() {
        return state_->database.contains(GStrToCStr(key));
    });
    return contains.value_or(false);
}

String
PDJE_KeyValueDB::GetText(String key)
{
    if (!RequireOpen("PDJE_KeyValueDB.GetText", state_)) {
        return String();
    }

    auto text = value_or_error("PDJE_KeyValueDB.GetText", [&]() {
        return state_->database.get_text(GStrToCStr(key));
    });
    return text.has_value() ? CStrToGStr(*text) : String();
}

PackedByteArray
PDJE_KeyValueDB::GetBytes(String key)
{
    if (!RequireOpen("PDJE_KeyValueDB.GetBytes", state_)) {
        return PackedByteArray();
    }

    auto bytes = value_or_error("PDJE_KeyValueDB.GetBytes", [&]() {
        return state_->database.get_bytes(GStrToCStr(key));
    });
    return bytes.has_value() ? BytesToPackedByteArray(*bytes)
                             : PackedByteArray();
}

bool
PDJE_KeyValueDB::TryGetBytesSilently(String          key,
                                     PackedByteArray &out_value,
                                     bool            &out_found) const
{
    out_value = PackedByteArray();
    out_found = false;

    if (state_ == nullptr || !state_->database.is_open) {
        return false;
    }

    try {
        const auto native_key = GStrToCStr(key);
        if (!state_->database.contains(native_key)) {
            return true;
        }
        out_value =
            BytesToPackedByteArray(state_->database.get_bytes(native_key));
        out_found = true;
        return true;
    } catch (...) {
        return false;
    }
}

bool
PDJE_KeyValueDB::PutText(String key, String value)
{
    if (!RequireOpen("PDJE_KeyValueDB.PutText", state_)) {
        return false;
    }

    return call_or_error("PDJE_KeyValueDB.PutText", [&]() {
        state_->database.put_text(GStrToCStr(key), GStrToCStr(value));
    });
}

bool
PDJE_KeyValueDB::PutBytes(String key, PackedByteArray value)
{
    if (!RequireOpen("PDJE_KeyValueDB.PutBytes", state_)) {
        return false;
    }

    const auto bytes = PackedByteArrayToBytes(value);
    return call_or_error("PDJE_KeyValueDB.PutBytes", [&]() {
        state_->database.put_bytes(
            GStrToCStr(key),
            std::span<const std::byte>(bytes.data(), bytes.size()));
    });
}

bool
PDJE_KeyValueDB::Erase(String key)
{
    if (!RequireOpen("PDJE_KeyValueDB.Erase", state_)) {
        return false;
    }

    return call_or_error("PDJE_KeyValueDB.Erase", [&]() {
        state_->database.erase(GStrToCStr(key));
    });
}

PackedStringArray
PDJE_KeyValueDB::ListKeys(String prefix)
{
    if (!RequireOpen("PDJE_KeyValueDB.ListKeys", state_)) {
        return PackedStringArray();
    }

    auto keys = value_or_error("PDJE_KeyValueDB.ListKeys", [&]() {
        return state_->database.list_keys(GStrToCStr(prefix));
    });
    return keys.has_value() ? KeysToPackedStringArray(*keys)
                            : PackedStringArray();
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
