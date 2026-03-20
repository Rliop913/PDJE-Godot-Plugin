#include "util/db/relational/adapter/LowLevelRelationalAdapter.hpp"

#include "util/common/bridge/LowLevelUtilCommon.hpp"

#include <utility>

namespace {

using godot::Array;
using godot::Dictionary;
using godot::PackedByteArray;
using godot::PackedStringArray;
using godot::String;
using godot::Variant;
using godot::pdje_low_level_util::common::BytesToPackedByteArray;
using godot::pdje_low_level_util::common::MakeStatusResult;
using godot::pdje_low_level_util::common::MakeSuccessResult;
using godot::pdje_low_level_util::common::PackedByteArrayToBytes;
using godot::pdje_low_level_util::common::VariantToBool;
using RelationalState = godot::pdje_low_level_util_internal::RelationalState;

constexpr const char *RELATIONAL_CLOSED_MESSAGE =
    "Relational database is not open.";

struct RelationalConfigParseResult {
    PDJE_UTIL::db::backends::SqliteConfig config;
    Dictionary                            normalized;
    PDJE_UTIL::common::Status             status;
};

struct RelationalParamsParseResult {
    PDJE_UTIL::db::relational::Params params;
    Array                             normalized;
    PDJE_UTIL::common::Status         status;
};

Dictionary
MakeRelationalExecPayload()
{
    Dictionary payload;
    payload["affected_rows"]     = static_cast<int64_t>(0);
    payload["last_insert_rowid"] = Variant();
    return payload;
}

Dictionary
MakeRelationalQueryPayload()
{
    Dictionary payload;
    payload["rows"] = Array();
    return payload;
}

Dictionary
MakeRelationalData(const Dictionary &base,
                   const String     &path,
                   const Dictionary &config,
                   bool              is_open,
                   const Variant    &sql,
                   const Variant    &params,
                   const Variant    &action)
{
    Dictionary data = base;
    data["path"]    = path;
    data["config"]  = config;
    data["is_open"] = is_open;

    if (sql.get_type() != Variant::NIL) {
        data["sql"] = sql;
    }
    if (params.get_type() != Variant::NIL) {
        data["params"] = params;
    }
    if (action.get_type() != Variant::NIL) {
        data["action"] = action;
    }

    return data;
}

bool
IsOpen(const std::unique_ptr<RelationalState> &state)
{
    return state != nullptr && state->is_open;
}

RelationalConfigParseResult
ParseRelationalConfig(const Dictionary &input)
{
    RelationalConfigParseResult parsed;
    parsed.normalized["path"]               = String();
    parsed.normalized["create_if_missing"]  = false;
    parsed.normalized["truncate_if_exists"] = false;
    parsed.normalized["read_only"]          = false;

    if (!input.has("path")) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Relational config requires a non-empty 'path'." };
        return parsed;
    }
    if (input["path"].get_type() != Variant::STRING) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Relational config 'path' must be a String." };
        return parsed;
    }

    const String path = static_cast<String>(input["path"]);
    if (path.is_empty()) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Relational config requires a non-empty 'path'." };
        return parsed;
    }

    Dictionary open_options;
    if (input.has("open_options")) {
        if (input["open_options"].get_type() != Variant::DICTIONARY) {
            parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                              "Relational config 'open_options' must be a "
                              "Dictionary when provided." };
            return parsed;
        }
        open_options = static_cast<Dictionary>(input["open_options"]);
    }

    auto parse_open_bool = [&](const char *key, bool default_value) {
        const std::string key_string(key);
        bool              value = default_value;
        if (input.has(key)) {
            if (!VariantToBool(input[key], value)) {
                return std::optional<PDJE_UTIL::common::Status>{
                    PDJE_UTIL::common::Status{
                        PDJE_UTIL::common::StatusCode::invalid_argument,
                        "Relational config '" + key_string +
                            "' must be a bool when provided." }
                };
            }
        } else if (open_options.has(key)) {
            if (!VariantToBool(open_options[key], value)) {
                return std::optional<PDJE_UTIL::common::Status>{
                    PDJE_UTIL::common::Status{
                        PDJE_UTIL::common::StatusCode::invalid_argument,
                        "Relational config open_options['" + key_string +
                            "'] must be a bool when provided." }
                };
            }
        }
        parsed.normalized[key] = value;
        return std::optional<PDJE_UTIL::common::Status>{};
    };

    if (auto status = parse_open_bool("create_if_missing", false);
        status.has_value()) {
        parsed.status = std::move(*status);
        return parsed;
    }
    if (auto status = parse_open_bool("truncate_if_exists", false);
        status.has_value()) {
        parsed.status = std::move(*status);
        return parsed;
    }
    if (auto status = parse_open_bool("read_only", false); status.has_value()) {
        parsed.status = std::move(*status);
        return parsed;
    }

    parsed.config.path         = GpathToCPath(path);
    parsed.config.open_options = {
        .create_if_missing =
            static_cast<bool>(parsed.normalized["create_if_missing"]),
        .truncate_if_exists =
            static_cast<bool>(parsed.normalized["truncate_if_exists"]),
        .read_only = static_cast<bool>(parsed.normalized["read_only"])
    };

    parsed.normalized["path"] = path;
    parsed.status             = {};
    return parsed;
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
        return Variant(
            CStrToGStr(std::get<PDJE_UTIL::db::Text>(value.storage)));
    case PDJE_UTIL::db::relational::ValueKind::bytes:
        return Variant(BytesToPackedByteArray(
            std::get<PDJE_UTIL::db::Bytes>(value.storage)));
    }

    return Variant();
}

RelationalParamsParseResult
ParseParams(const Array &input)
{
    RelationalParamsParseResult parsed;
    parsed.params.reserve(static_cast<size_t>(input.size()));

    for (int64_t i = 0; i < input.size(); ++i) {
        const Variant value = input[i];
        switch (value.get_type()) {
        case Variant::NIL:
            parsed.params.push_back({ PDJE_UTIL::db::relational::ValueStorage{
                std::monostate{} } });
            parsed.normalized.append(Variant());
            break;
        case Variant::INT: {
            const int64_t integer_value = static_cast<int64_t>(value);
            parsed.params.push_back(
                { PDJE_UTIL::db::relational::ValueStorage{ integer_value } });
            parsed.normalized.append(integer_value);
            break;
        }
        case Variant::FLOAT: {
            const double float_value = static_cast<double>(value);
            parsed.params.push_back(
                { PDJE_UTIL::db::relational::ValueStorage{ float_value } });
            parsed.normalized.append(float_value);
            break;
        }
        case Variant::STRING: {
            const String string_value = static_cast<String>(value);
            parsed.params.push_back({ PDJE_UTIL::db::relational::ValueStorage{
                GStrToCStr(string_value) } });
            parsed.normalized.append(string_value);
            break;
        }
        case Variant::PACKED_BYTE_ARRAY: {
            const PackedByteArray bytes_value =
                static_cast<PackedByteArray>(value);
            parsed.params.push_back({ PDJE_UTIL::db::relational::ValueStorage{
                PackedByteArrayToBytes(bytes_value) } });
            parsed.normalized.append(bytes_value);
            break;
        }
        default:
            parsed.status = {
                PDJE_UTIL::common::StatusCode::invalid_argument,
                "Relational params must contain only null, int, float, "
                "String, or PackedByteArray values."
            };
            parsed.params.clear();
            parsed.normalized = Array();
            return parsed;
        }
    }

    parsed.status = {};
    return parsed;
}

Array
MakeQueryRows(const PDJE_UTIL::db::relational::QueryResult &query_result)
{
    Array rows;
    for (const auto &row : query_result.rows) {
        Dictionary        normalized_row;
        PackedStringArray columns;
        Array             values;

        for (const auto &column : row.columns) {
            columns.append(CStrToGStr(column));
        }
        for (const auto &value : row.values) {
            values.append(MakeGodotValue(value));
        }

        normalized_row["columns"] = columns;
        normalized_row["values"]  = values;
        rows.append(normalized_row);
    }
    return rows;
}

Dictionary
MakeClosedResult(const std::unique_ptr<RelationalState> &state,
                 const Dictionary                       &payload,
                 const Variant                          &sql,
                 const Variant                          &params,
                 const Variant                          &action)
{
    const PDJE_UTIL::common::Status status{
        PDJE_UTIL::common::StatusCode::closed, RELATIONAL_CLOSED_MESSAGE
    };
    return MakeStatusResult(
        false,
        status,
        MakeRelationalData(payload,
                           state != nullptr ? state->path : String(),
                           state != nullptr ? state->config : Dictionary(),
                           false,
                           sql,
                           params,
                           action));
}

Dictionary
MakeTransactionPayload(const String &action)
{
    Dictionary payload           = MakeRelationalExecPayload();
    payload["action"]            = action;
    payload["affected_rows"]     = static_cast<int64_t>(0);
    payload["last_insert_rowid"] = Variant();
    return payload;
}

} // namespace

namespace godot::pdje_low_level_util::relational {

Dictionary
Create(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
       const Dictionary                                               &config)
{
    auto parsed = ParseRelationalConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeRelationalData(MakeRelationalExecPayload(),
                                                   String(),
                                                   parsed.normalized,
                                                   IsOpen(state),
                                                   Variant(),
                                                   Variant(),
                                                   Variant()));
    }

    auto created =
        PDJE_UTIL::db::backends::SqliteBackend::create(parsed.config);
    if (!created.ok()) {
        return MakeStatusResult(
            false,
            created.status(),
            MakeRelationalData(MakeRelationalExecPayload(),
                               static_cast<String>(parsed.normalized["path"]),
                               parsed.normalized,
                               IsOpen(state),
                               Variant(),
                               Variant(),
                               Variant()));
    }

    return MakeSuccessResult(
        MakeRelationalData(MakeRelationalExecPayload(),
                           static_cast<String>(parsed.normalized["path"]),
                           parsed.normalized,
                           IsOpen(state),
                           Variant(),
                           Variant(),
                           Variant()));
}

Dictionary
Destroy(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
        const Dictionary                                               &config)
{
    auto parsed = ParseRelationalConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeRelationalData(MakeRelationalExecPayload(),
                                                   String(),
                                                   parsed.normalized,
                                                   IsOpen(state),
                                                   Variant(),
                                                   Variant(),
                                                   Variant()));
    }

    if (IsOpen(state) &&
        state->path == static_cast<String>(parsed.normalized["path"])) {
        auto closed = state->database.close();
        if (!closed.ok()) {
            return MakeStatusResult(
                false,
                closed.status(),
                MakeRelationalData(MakeRelationalExecPayload(),
                                   state->path,
                                   state->config,
                                   true,
                                   Variant(),
                                   Variant(),
                                   Variant()));
        }
        state->config  = Dictionary();
        state->path    = String();
        state->is_open = false;
    }

    auto destroyed =
        PDJE_UTIL::db::backends::SqliteBackend::destroy(parsed.config);
    if (!destroyed.ok()) {
        return MakeStatusResult(
            false,
            destroyed.status(),
            MakeRelationalData(MakeRelationalExecPayload(),
                               static_cast<String>(parsed.normalized["path"]),
                               parsed.normalized,
                               IsOpen(state),
                               Variant(),
                               Variant(),
                               Variant()));
    }

    return MakeSuccessResult(
        MakeRelationalData(MakeRelationalExecPayload(),
                           static_cast<String>(parsed.normalized["path"]),
                           parsed.normalized,
                           IsOpen(state),
                           Variant(),
                           Variant(),
                           Variant()));
}

Dictionary
Open(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
     const Dictionary                                               &config)
{
    auto parsed = ParseRelationalConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeRelationalData(MakeRelationalExecPayload(),
                                                   String(),
                                                   parsed.normalized,
                                                   IsOpen(state),
                                                   Variant(),
                                                   Variant(),
                                                   Variant()));
    }

    if (IsOpen(state)) {
        const PDJE_UTIL::common::Status status{
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "Relational database is already open. Close it before opening "
            "another one."
        };
        return MakeStatusResult(false,
                                status,
                                MakeRelationalData(MakeRelationalExecPayload(),
                                                   state->path,
                                                   state->config,
                                                   true,
                                                   Variant(),
                                                   Variant(),
                                                   Variant()));
    }

    if (state == nullptr) {
        state = std::make_unique<RelationalState>();
    }

    auto opened = pdje_low_level_util_internal::NativeRelationalDatabase::open(
        parsed.config);
    if (!opened.ok()) {
        return MakeStatusResult(
            false,
            opened.status(),
            MakeRelationalData(MakeRelationalExecPayload(),
                               static_cast<String>(parsed.normalized["path"]),
                               parsed.normalized,
                               false,
                               Variant(),
                               Variant(),
                               Variant()));
    }

    state->database = std::move(opened.value());
    state->config   = parsed.normalized;
    state->path     = static_cast<String>(parsed.normalized["path"]);
    state->is_open  = true;

    return MakeSuccessResult(MakeRelationalData(MakeRelationalExecPayload(),
                                                state->path,
                                                state->config,
                                                true,
                                                Variant(),
                                                Variant(),
                                                Variant()));
}

Dictionary
Close(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                MakeRelationalExecPayload(),
                                Variant(),
                                Variant(),
                                Variant());
    }

    auto closed = state->database.close();
    if (!closed.ok()) {
        return MakeStatusResult(false,
                                closed.status(),
                                MakeRelationalData(MakeRelationalExecPayload(),
                                                   state->path,
                                                   state->config,
                                                   true,
                                                   Variant(),
                                                   Variant(),
                                                   Variant()));
    }

    Dictionary config_copy = state->config;
    String     path_copy   = state->path;
    state->config          = Dictionary();
    state->path            = String();
    state->is_open         = false;

    return MakeSuccessResult(MakeRelationalData(MakeRelationalExecPayload(),
                                                path_copy,
                                                config_copy,
                                                false,
                                                Variant(),
                                                Variant(),
                                                Variant()));
}

Dictionary
Execute(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
        const String                                                   &sql,
        const Array                                                    &params)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(
            state, MakeRelationalExecPayload(), sql, params, Variant());
    }

    auto parsed_params = ParseParams(params);
    if (!parsed_params.status.ok()) {
        return MakeStatusResult(false,
                                parsed_params.status,
                                MakeRelationalData(MakeRelationalExecPayload(),
                                                   state->path,
                                                   state->config,
                                                   true,
                                                   sql,
                                                   parsed_params.normalized,
                                                   Variant()));
    }

    auto executed =
        state->database.execute(GStrToCStr(sql), parsed_params.params);
    if (!executed.ok()) {
        return MakeStatusResult(false,
                                executed.status(),
                                MakeRelationalData(MakeRelationalExecPayload(),
                                                   state->path,
                                                   state->config,
                                                   true,
                                                   sql,
                                                   parsed_params.normalized,
                                                   Variant()));
    }

    Dictionary payload = MakeRelationalExecPayload();
    payload["affected_rows"] =
        static_cast<int64_t>(executed.value().affected_rows);
    payload["last_insert_rowid"] =
        executed.value().last_insert_rowid.has_value()
            ? Variant(*executed.value().last_insert_rowid)
            : Variant();

    return MakeSuccessResult(MakeRelationalData(payload,
                                                state->path,
                                                state->config,
                                                true,
                                                sql,
                                                parsed_params.normalized,
                                                Variant()));
}

Dictionary
Query(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
      const String                                                   &sql,
      const Array                                                    &params)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(
            state, MakeRelationalQueryPayload(), sql, params, Variant());
    }

    auto parsed_params = ParseParams(params);
    if (!parsed_params.status.ok()) {
        return MakeStatusResult(false,
                                parsed_params.status,
                                MakeRelationalData(MakeRelationalQueryPayload(),
                                                   state->path,
                                                   state->config,
                                                   true,
                                                   sql,
                                                   parsed_params.normalized,
                                                   Variant()));
    }

    auto queried = state->database.query(GStrToCStr(sql), parsed_params.params);
    if (!queried.ok()) {
        return MakeStatusResult(false,
                                queried.status(),
                                MakeRelationalData(MakeRelationalQueryPayload(),
                                                   state->path,
                                                   state->config,
                                                   true,
                                                   sql,
                                                   parsed_params.normalized,
                                                   Variant()));
    }

    Dictionary payload;
    payload["rows"] = MakeQueryRows(queried.value());

    return MakeSuccessResult(MakeRelationalData(payload,
                                                state->path,
                                                state->config,
                                                true,
                                                sql,
                                                parsed_params.normalized,
                                                Variant()));
}

Dictionary
BeginTransaction(
    std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state)
{
    const String action = "begin";
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                MakeTransactionPayload(action),
                                Variant(),
                                Variant(),
                                action);
    }

    auto begun = state->database.begin_transaction();
    if (!begun.ok()) {
        return MakeStatusResult(
            false,
            begun.status(),
            MakeRelationalData(MakeTransactionPayload(action),
                               state->path,
                               state->config,
                               true,
                               Variant(),
                               Variant(),
                               action));
    }

    return MakeSuccessResult(MakeRelationalData(MakeTransactionPayload(action),
                                                state->path,
                                                state->config,
                                                true,
                                                Variant(),
                                                Variant(),
                                                action));
}

Dictionary
Commit(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state)
{
    const String action = "commit";
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                MakeTransactionPayload(action),
                                Variant(),
                                Variant(),
                                action);
    }

    auto committed = state->database.commit();
    if (!committed.ok()) {
        return MakeStatusResult(
            false,
            committed.status(),
            MakeRelationalData(MakeTransactionPayload(action),
                               state->path,
                               state->config,
                               true,
                               Variant(),
                               Variant(),
                               action));
    }

    return MakeSuccessResult(MakeRelationalData(MakeTransactionPayload(action),
                                                state->path,
                                                state->config,
                                                true,
                                                Variant(),
                                                Variant(),
                                                action));
}

Dictionary
Rollback(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state)
{
    const String action = "rollback";
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                MakeTransactionPayload(action),
                                Variant(),
                                Variant(),
                                action);
    }

    auto rolled_back = state->database.rollback();
    if (!rolled_back.ok()) {
        return MakeStatusResult(
            false,
            rolled_back.status(),
            MakeRelationalData(MakeTransactionPayload(action),
                               state->path,
                               state->config,
                               true,
                               Variant(),
                               Variant(),
                               action));
    }

    return MakeSuccessResult(MakeRelationalData(MakeTransactionPayload(action),
                                                state->path,
                                                state->config,
                                                true,
                                                Variant(),
                                                Variant(),
                                                action));
}

void
Shutdown(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state)
{
    if (IsOpen(state)) {
        (void)state->database.close();
    }
}

} // namespace godot::pdje_low_level_util::relational
