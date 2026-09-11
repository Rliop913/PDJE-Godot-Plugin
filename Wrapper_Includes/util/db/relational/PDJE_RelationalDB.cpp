#include "util/db/relational/PDJE_RelationalDB.hpp"

#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"
#include "util/db/backends/SqliteBackend.hpp"
#include "util/db/relational/Database.hpp"
#include "util/db/relational/PDJE_RelationalTypes.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <cstdint>
#include <memory>
#include <optional>
#include <utility>
#include <variant>

using namespace godot;

namespace godot::pdje_relational_db_internal {

using NativeRelationalDatabase = PDJE_UTIL::db::relational::RelationalDatabase<
    PDJE_UTIL::db::backends::SqliteBackend>;

struct State {
    NativeRelationalDatabase database;
    String                   path;
};

} // namespace godot::pdje_relational_db_internal

namespace {

using godot::pdje_low_level_util::common::BytesToPackedByteArray;
using godot::pdje_low_level_util::common::PackedByteArrayToBytes;
using godot::pdje_public_util::common::call_or_error;
using godot::pdje_public_util::common::print_method_error;
using godot::pdje_public_util::common::value_or_error;
using godot::pdje_relational_db_internal::NativeRelationalDatabase;
using godot::pdje_relational_db_internal::State;

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

    print_method_error(method_name, "Relational database is not open");
    return false;
}

PDJE_UTIL::db::backends::SqliteConfig
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

Variant
MakeGodotValue(const PDJE_UTIL::db::relational::Value &value)
{
    if (std::holds_alternative<std::monostate>(value.storage)) {
        return Variant();
    }
    if (const auto *integer = std::get_if<std::int64_t>(&value.storage)) {
        return Variant(static_cast<int64_t>(*integer));
    }
    if (const auto *real = std::get_if<double>(&value.storage)) {
        return Variant(*real);
    }
    if (const auto *text = std::get_if<PDJE_UTIL::db::Text>(&value.storage)) {
        return Variant(CStrToGStr(*text));
    }
    if (const auto *bytes = std::get_if<PDJE_UTIL::db::Bytes>(&value.storage)) {
        return Variant(BytesToPackedByteArray(*bytes));
    }
    return Variant();
}

bool
ParseParams(const char                         *method_name,
            const Array                        &input,
            PDJE_UTIL::db::relational::Params &out_params)
{
    out_params.clear();
    out_params.reserve(static_cast<std::size_t>(input.size()));

    for (int64_t i = 0; i < input.size(); ++i) {
        const Variant value = input[i];
        switch (value.get_type()) {
        case Variant::NIL:
            out_params.push_back(
                { PDJE_UTIL::db::relational::ValueStorage{ std::monostate{} } });
            break;
        case Variant::INT:
            out_params.push_back({ PDJE_UTIL::db::relational::ValueStorage{
                static_cast<int64_t>(value) } });
            break;
        case Variant::FLOAT:
            out_params.push_back({ PDJE_UTIL::db::relational::ValueStorage{
                static_cast<double>(value) } });
            break;
        case Variant::STRING:
            out_params.push_back({ PDJE_UTIL::db::relational::ValueStorage{
                GStrToCStr(static_cast<String>(value)) } });
            break;
        case Variant::PACKED_BYTE_ARRAY:
            out_params.push_back({ PDJE_UTIL::db::relational::ValueStorage{
                PackedByteArrayToBytes(static_cast<PackedByteArray>(value)) } });
            break;
        default:
            print_method_error(
                method_name,
                "params must contain only nil, int, float, String, or PackedByteArray");
            out_params.clear();
            return false;
        }
    }

    return true;
}

Ref<PDJE_RelationalExecResult>
MakeExecResultRef(const PDJE_UTIL::db::relational::ExecResult &result)
{
    Ref<PDJE_RelationalExecResult> ref;
    ref.instantiate();
    ref->set_affected_rows(static_cast<int64_t>(result.affected_rows));
    ref->set_has_last_insert_rowid(result.last_insert_rowid.has_value());
    if (result.last_insert_rowid.has_value()) {
        ref->set_last_insert_rowid(*result.last_insert_rowid);
    }
    return ref;
}

Ref<PDJE_RelationalRow>
MakeRowRef(const PDJE_UTIL::db::relational::Row &row)
{
    Ref<PDJE_RelationalRow> ref;
    ref.instantiate();

    PackedStringArray columns;
    for (const auto &column : row.columns) {
        columns.append(CStrToGStr(column));
    }

    Array values;
    for (const auto &value : row.values) {
        values.append(MakeGodotValue(value));
    }

    ref->set_columns(columns);
    ref->set_values(values);
    return ref;
}

} // namespace

void
PDJE_RelationalDB::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("Create", "path", "truncate_if_exists"),
                         &PDJE_RelationalDB::Create,
                         DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Destroy", "path"),
                         &PDJE_RelationalDB::Destroy);
    ClassDB::bind_method(D_METHOD("Open",
                                  "path",
                                  "create_if_missing",
                                  "truncate_if_exists",
                                  "read_only"),
                         &PDJE_RelationalDB::Open,
                         DEFVAL(false),
                         DEFVAL(false),
                         DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Close"), &PDJE_RelationalDB::Close);
    ClassDB::bind_method(D_METHOD("IsOpen"), &PDJE_RelationalDB::IsOpen);
    ClassDB::bind_method(D_METHOD("GetPath"), &PDJE_RelationalDB::GetPath);
    ClassDB::bind_method(D_METHOD("Execute", "sql", "params"),
                         &PDJE_RelationalDB::Execute,
                         DEFVAL(Array()));
    ClassDB::bind_method(D_METHOD("Query", "sql", "params"),
                         &PDJE_RelationalDB::Query,
                         DEFVAL(Array()));
    ClassDB::bind_method(D_METHOD("BeginTransaction"),
                         &PDJE_RelationalDB::BeginTransaction);
    ClassDB::bind_method(D_METHOD("Commit"), &PDJE_RelationalDB::Commit);
    ClassDB::bind_method(D_METHOD("Rollback"), &PDJE_RelationalDB::Rollback);
}

bool
PDJE_RelationalDB::Create(String path, bool truncate_if_exists)
{
    if (!ValidatePath("PDJE_RelationalDB.Create", path)) {
        return false;
    }

    const auto config = MakeConfig(path, false, false, false);
    return call_or_error("PDJE_RelationalDB.Create", [&]() {
        if (truncate_if_exists) {
            NativeRelationalDatabase::destroy(config);
        }
        NativeRelationalDatabase::create(config);
    });
}

bool
PDJE_RelationalDB::Destroy(String path)
{
    if (!ValidatePath("PDJE_RelationalDB.Destroy", path)) {
        return false;
    }

    if (state_ != nullptr && state_->database.is_open &&
        state_->path == path &&
        !CloseState("PDJE_RelationalDB.Destroy", state_)) {
        return false;
    }

    return call_or_error("PDJE_RelationalDB.Destroy", [&]() {
        NativeRelationalDatabase::destroy(
            MakeConfig(path, false, false, false));
    });
}

bool
PDJE_RelationalDB::Open(String path,
                        bool   create_if_missing,
                        bool   truncate_if_exists,
                        bool   read_only)
{
    if (!ValidatePath("PDJE_RelationalDB.Open", path)) {
        return false;
    }
    if (state_ != nullptr && state_->database.is_open) {
        print_method_error("PDJE_RelationalDB.Open",
                           "Relational database is already open");
        return false;
    }

    if (state_ == nullptr) {
        state_ = std::make_unique<State>();
    }

    auto opened = value_or_error("PDJE_RelationalDB.Open", [&]() {
        return NativeRelationalDatabase::open(MakeConfig(path,
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
PDJE_RelationalDB::Close()
{
    if (!RequireOpen("PDJE_RelationalDB.Close", state_)) {
        return false;
    }
    return CloseState("PDJE_RelationalDB.Close", state_);
}

bool
PDJE_RelationalDB::IsOpen() const
{
    return state_ != nullptr && state_->database.is_open;
}

String
PDJE_RelationalDB::GetPath() const
{
    return state_ != nullptr ? state_->path : String();
}

Ref<PDJE_RelationalExecResult>
PDJE_RelationalDB::Execute(String sql, Array params)
{
    if (!RequireOpen("PDJE_RelationalDB.Execute", state_)) {
        return Ref<PDJE_RelationalExecResult>();
    }

    PDJE_UTIL::db::relational::Params native_params;
    if (!ParseParams("PDJE_RelationalDB.Execute", params, native_params)) {
        return Ref<PDJE_RelationalExecResult>();
    }

    auto executed = value_or_error("PDJE_RelationalDB.Execute", [&]() {
        return state_->database.execute(GStrToCStr(sql), native_params);
    });
    return executed.has_value() ? MakeExecResultRef(*executed)
                                : Ref<PDJE_RelationalExecResult>();
}

Array
PDJE_RelationalDB::Query(String sql, Array params)
{
    if (!RequireOpen("PDJE_RelationalDB.Query", state_)) {
        return Array();
    }

    PDJE_UTIL::db::relational::Params native_params;
    if (!ParseParams("PDJE_RelationalDB.Query", params, native_params)) {
        return Array();
    }

    auto queried = value_or_error("PDJE_RelationalDB.Query", [&]() {
        return state_->database.query(GStrToCStr(sql), native_params);
    });
    if (!queried.has_value()) {
        return Array();
    }

    Array rows;
    for (const auto &row : queried->rows) {
        rows.append(MakeRowRef(row));
    }
    return rows;
}

bool
PDJE_RelationalDB::BeginTransaction()
{
    if (!RequireOpen("PDJE_RelationalDB.BeginTransaction", state_)) {
        return false;
    }

    return call_or_error("PDJE_RelationalDB.BeginTransaction",
                         [&]() { state_->database.begin_transaction(); });
}

bool
PDJE_RelationalDB::Commit()
{
    if (!RequireOpen("PDJE_RelationalDB.Commit", state_)) {
        return false;
    }

    return call_or_error("PDJE_RelationalDB.Commit",
                         [&]() { state_->database.commit(); });
}

bool
PDJE_RelationalDB::Rollback()
{
    if (!RequireOpen("PDJE_RelationalDB.Rollback", state_)) {
        return false;
    }

    return call_or_error("PDJE_RelationalDB.Rollback",
                         [&]() { state_->database.rollback(); });
}

PDJE_RelationalDB::PDJE_RelationalDB() = default;

PDJE_RelationalDB::~PDJE_RelationalDB()
{
    (void)CloseState("PDJE_RelationalDB.~PDJE_RelationalDB", state_);
}

void
PDJE_RelationalDB::_ready()
{
}
