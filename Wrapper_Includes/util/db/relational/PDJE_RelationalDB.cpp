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

using namespace godot;

namespace godot::pdje_relational_db_internal {

using NativeRelationalDatabase = PDJE_UTIL::db::relational::RelationalDatabase<
    PDJE_UTIL::db::backends::SqliteBackend>;

struct State {
    NativeRelationalDatabase database;
    String                   path;
    bool                     is_open = false;
};

} // namespace godot::pdje_relational_db_internal

namespace {

using godot::pdje_low_level_util::common::BytesToPackedByteArray;
using godot::pdje_low_level_util::common::PackedByteArrayToBytes;
using godot::pdje_low_level_util::common::StatusCodeToGodotCode;
using godot::pdje_low_level_util::common::StatusMessageToGodot;
using godot::pdje_public_util::common::print_method_error;
using godot::pdje_relational_db_internal::NativeRelationalDatabase;
using godot::pdje_relational_db_internal::State;

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
    state.path    = String();
    state.is_open = false;
}

bool
CloseState(const char *method_name, const std::unique_ptr<State> &state)
{
    if (state == nullptr || !state->is_open) {
        return true;
    }

    auto closed = state->database.close();
    if (!closed.ok()) {
        PrintStatusError(method_name, closed.status());
        return false;
    }

    ResetState(*state);
    return true;
}

Variant
MakeGodotValue(const PDJE_UTIL::db::relational::Value &value)
{
    switch (value.kind()) {
    case PDJE_UTIL::db::relational::ValueKind::null_value:
        return Variant();
    case PDJE_UTIL::db::relational::ValueKind::integer:
        return Variant(
            static_cast<int64_t>(std::get<std::int64_t>(value.storage)));
    case PDJE_UTIL::db::relational::ValueKind::real:
        return Variant(std::get<double>(value.storage));
    case PDJE_UTIL::db::relational::ValueKind::text:
        return Variant(CStrToGStr(std::get<PDJE_UTIL::db::Text>(value.storage)));
    case PDJE_UTIL::db::relational::ValueKind::bytes:
        return Variant(BytesToPackedByteArray(
            std::get<PDJE_UTIL::db::Bytes>(value.storage)));
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

    auto created = PDJE_UTIL::db::backends::SqliteBackend::create(
        MakeConfig(path, false, truncate_if_exists, false));
    if (!created.ok()) {
        PrintStatusError("PDJE_RelationalDB.Create", created.status());
        return false;
    }

    return true;
}

bool
PDJE_RelationalDB::Destroy(String path)
{
    if (!ValidatePath("PDJE_RelationalDB.Destroy", path)) {
        return false;
    }

    if (state_ != nullptr && state_->is_open && state_->path == path &&
        !CloseState("PDJE_RelationalDB.Destroy", state_)) {
        return false;
    }

    auto destroyed = PDJE_UTIL::db::backends::SqliteBackend::destroy(
        MakeConfig(path, false, false, false));
    if (!destroyed.ok()) {
        PrintStatusError("PDJE_RelationalDB.Destroy", destroyed.status());
        return false;
    }

    return true;
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
    if (state_ != nullptr && state_->is_open) {
        print_method_error("PDJE_RelationalDB.Open",
                           "Relational database is already open");
        return false;
    }

    if (state_ == nullptr) {
        state_ = std::make_unique<State>();
    }

    auto opened = NativeRelationalDatabase::open(MakeConfig(path,
                                                            create_if_missing,
                                                            truncate_if_exists,
                                                            read_only));
    if (!opened.ok()) {
        PrintStatusError("PDJE_RelationalDB.Open", opened.status());
        return false;
    }

    state_->database = std::move(opened.value());
    state_->path     = path;
    state_->is_open  = true;
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
    return state_ != nullptr && state_->is_open;
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

    auto executed = state_->database.execute(GStrToCStr(sql), native_params);
    if (!executed.ok()) {
        PrintStatusError("PDJE_RelationalDB.Execute", executed.status());
        return Ref<PDJE_RelationalExecResult>();
    }

    return MakeExecResultRef(executed.value());
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

    auto queried = state_->database.query(GStrToCStr(sql), native_params);
    if (!queried.ok()) {
        PrintStatusError("PDJE_RelationalDB.Query", queried.status());
        return Array();
    }

    Array rows;
    for (const auto &row : queried.value().rows) {
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

    auto begun = state_->database.begin_transaction();
    if (!begun.ok()) {
        PrintStatusError("PDJE_RelationalDB.BeginTransaction", begun.status());
        return false;
    }

    return true;
}

bool
PDJE_RelationalDB::Commit()
{
    if (!RequireOpen("PDJE_RelationalDB.Commit", state_)) {
        return false;
    }

    auto committed = state_->database.commit();
    if (!committed.ok()) {
        PrintStatusError("PDJE_RelationalDB.Commit", committed.status());
        return false;
    }

    return true;
}

bool
PDJE_RelationalDB::Rollback()
{
    if (!RequireOpen("PDJE_RelationalDB.Rollback", state_)) {
        return false;
    }

    auto rolled_back = state_->database.rollback();
    if (!rolled_back.ok()) {
        PrintStatusError("PDJE_RelationalDB.Rollback", rolled_back.status());
        return false;
    }

    return true;
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
