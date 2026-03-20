#include "util/db/nearest/adapter/LowLevelNearestAdapter.hpp"

#include "util/common/bridge/LowLevelUtilCommon.hpp"

#include <optional>
#include <span>
#include <utility>
#include <vector>

namespace {

using godot::Array;
using godot::Dictionary;
using godot::PackedByteArray;
using godot::PackedStringArray;
using godot::String;
using godot::Variant;
using godot::pdje_low_level_util::common::BytesToPackedByteArray;
using godot::pdje_low_level_util::common::KeysToPackedStringArray;
using godot::pdje_low_level_util::common::MakeStatusResult;
using godot::pdje_low_level_util::common::MakeSuccessResult;
using godot::pdje_low_level_util::common::PackedByteArrayToBytes;
using godot::pdje_low_level_util::common::VariantToBool;
using godot::pdje_low_level_util::common::VariantToFloat;
using godot::pdje_low_level_util::common::VariantToInteger;
using NearestState = godot::pdje_low_level_util_internal::NearestState;

constexpr const char *NEAREST_CLOSED_MESSAGE = "Nearest database is not open.";

struct NearestConfigParseResult {
    PDJE_UTIL::db::backends::AnnoyConfig config;
    Dictionary                           normalized;
    PDJE_UTIL::common::Status            status;
};

struct EmbeddingParseResult {
    std::vector<float>        values;
    Array                     normalized;
    PDJE_UTIL::common::Status status;
};

struct NearestItemParseResult {
    PDJE_UTIL::db::nearest::Item item;
    Dictionary                   normalized;
    PDJE_UTIL::common::Status    status;
};

struct NearestSearchOptionsParseResult {
    PDJE_UTIL::db::nearest::SearchOptions options;
    Dictionary                            normalized;
    PDJE_UTIL::common::Status             status;
};

Dictionary
MakeNearestItemPayload()
{
    Dictionary item;
    item["id"]            = String();
    item["embedding"]     = Array();
    item["text_payload"]  = String();
    item["bytes_payload"] = PackedByteArray();

    Dictionary payload;
    payload["item"] = item;
    return payload;
}

Dictionary
MakeNearestSearchPayload()
{
    Dictionary payload;
    payload["hits"] = Array();
    payload["keys"] = PackedStringArray();
    return payload;
}

Dictionary
MakeNearestData(const Dictionary &base,
                const String     &root_path,
                const Dictionary &config,
                bool              is_open,
                const Variant    &id,
                const Variant    &query_embedding,
                const Variant    &search_options,
                const Variant    &contains,
                const Variant    &item,
                const Variant    &hits,
                const Variant    &keys)
{
    Dictionary data   = base;
    data["root_path"] = root_path;
    data["config"]    = config;
    data["is_open"]   = is_open;

    if (id.get_type() != Variant::NIL) {
        data["id"] = id;
    }
    if (query_embedding.get_type() != Variant::NIL) {
        data["query_embedding"] = query_embedding;
    }
    if (search_options.get_type() != Variant::NIL) {
        data["search_options"] = search_options;
    }
    if (contains.get_type() != Variant::NIL) {
        data["contains"] = contains;
    }
    if (item.get_type() != Variant::NIL) {
        data["item"] = item;
    }
    if (hits.get_type() != Variant::NIL) {
        data["hits"] = hits;
    }
    if (keys.get_type() != Variant::NIL) {
        data["keys"] = keys;
    }

    return data;
}

EmbeddingParseResult
ParseEmbeddingArray(const Array &input,
                    const char  *field_name,
                    int64_t      expected_dimension = -1)
{
    EmbeddingParseResult parsed;
    const std::string    field_name_string(field_name);

    if (input.is_empty()) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          field_name_string +
                              " must be a non-empty Array of floats." };
        return parsed;
    }

    parsed.values.reserve(static_cast<size_t>(input.size()));
    for (int64_t i = 0; i < input.size(); ++i) {
        float value = 0.0F;
        if (!VariantToFloat(input[i], value)) {
            parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                              field_name_string +
                                  " must contain only numeric values." };
            parsed.values.clear();
            parsed.normalized = Array();
            return parsed;
        }
        parsed.values.push_back(value);
        parsed.normalized.append(value);
    }

    if (expected_dimension > 0 &&
        static_cast<int64_t>(parsed.values.size()) != expected_dimension) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          field_name_string +
                              " size must match the configured dimension." };
        return parsed;
    }

    parsed.status = {};
    return parsed;
}

NearestConfigParseResult
ParseNearestConfig(const Dictionary &input)
{
    NearestConfigParseResult parsed;
    parsed.normalized["root_path"]          = String();
    parsed.normalized["dimension"]          = static_cast<int64_t>(0);
    parsed.normalized["trees"]              = static_cast<int64_t>(10);
    parsed.normalized["prefault"]           = false;
    parsed.normalized["create_if_missing"]  = false;
    parsed.normalized["truncate_if_exists"] = false;
    parsed.normalized["read_only"]          = false;

    if (!input.has("root_path")) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Nearest config requires a non-empty 'root_path'." };
        return parsed;
    }

    if (input["root_path"].get_type() != Variant::STRING) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Nearest config 'root_path' must be a String." };
        return parsed;
    }

    const String root_path = static_cast<String>(input["root_path"]);
    if (root_path.is_empty()) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Nearest config requires a non-empty 'root_path'." };
        return parsed;
    }

    if (!input.has("dimension")) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Nearest config requires a positive 'dimension'." };
        return parsed;
    }

    int64_t dimension = 0;
    if (!VariantToInteger(input["dimension"], dimension) || dimension <= 0) {
        parsed.status = {
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "Nearest config 'dimension' must be a positive integer."
        };
        return parsed;
    }

    int64_t trees = 10;
    if (input.has("trees")) {
        if (!VariantToInteger(input["trees"], trees) || trees <= 0) {
            parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                              "Nearest config 'trees' must be a positive "
                              "integer when provided." };
            return parsed;
        }
    }

    bool prefault = false;
    if (input.has("prefault") && !VariantToBool(input["prefault"], prefault)) {
        parsed.status = {
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "Nearest config 'prefault' must be a bool when provided."
        };
        return parsed;
    }

    Dictionary open_options;
    if (input.has("open_options")) {
        if (input["open_options"].get_type() != Variant::DICTIONARY) {
            parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                              "Nearest config 'open_options' must be a "
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
                        "Nearest config '" + key_string +
                            "' must be a bool when provided." }
                };
            }
        } else if (open_options.has(key)) {
            if (!VariantToBool(open_options[key], value)) {
                return std::optional<PDJE_UTIL::common::Status>{
                    PDJE_UTIL::common::Status{
                        PDJE_UTIL::common::StatusCode::invalid_argument,
                        "Nearest config open_options['" + key_string +
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

    parsed.config.root_path    = GpathToCPath(root_path);
    parsed.config.open_options = {
        .create_if_missing =
            static_cast<bool>(parsed.normalized["create_if_missing"]),
        .truncate_if_exists =
            static_cast<bool>(parsed.normalized["truncate_if_exists"]),
        .read_only = static_cast<bool>(parsed.normalized["read_only"])
    };
    parsed.config.dimension = static_cast<size_t>(dimension);
    parsed.config.trees     = static_cast<int>(trees);
    parsed.config.prefault  = prefault;

    parsed.normalized["root_path"] = root_path;
    parsed.normalized["dimension"] = dimension;
    parsed.normalized["trees"]     = trees;
    parsed.normalized["prefault"]  = prefault;
    parsed.status                  = {};
    return parsed;
}

Dictionary
MakeNearestItemDictionary(const PDJE_UTIL::db::nearest::Item &item)
{
    Dictionary result;
    result["id"] = CStrToGStr(item.id);

    Array embedding;
    for (float value : item.embedding) {
        embedding.append(value);
    }
    result["embedding"] = embedding;

    result["text_payload"] = item.text_payload.has_value()
                                 ? Variant(CStrToGStr(*item.text_payload))
                                 : Variant(String());
    result["bytes_payload"] =
        item.bytes_payload.has_value()
            ? Variant(BytesToPackedByteArray(*item.bytes_payload))
            : Variant(PackedByteArray());
    return result;
}

Dictionary
MakeNearestHitDictionary(const PDJE_UTIL::db::nearest::SearchHit &hit)
{
    Dictionary result;
    result["id"]           = CStrToGStr(hit.id);
    result["distance"]     = hit.distance;
    result["text_payload"] = hit.text_payload.has_value()
                                 ? Variant(CStrToGStr(*hit.text_payload))
                                 : Variant(String());
    result["bytes_payload"] =
        hit.bytes_payload.has_value()
            ? Variant(BytesToPackedByteArray(*hit.bytes_payload))
            : Variant(PackedByteArray());
    return result;
}

NearestItemParseResult
ParseNearestItem(const Dictionary &input, int64_t expected_dimension)
{
    NearestItemParseResult parsed;
    parsed.normalized["id"]            = String();
    parsed.normalized["embedding"]     = Array();
    parsed.normalized["text_payload"]  = String();
    parsed.normalized["bytes_payload"] = PackedByteArray();

    if (!input.has("id")) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Nearest item requires a non-empty 'id'." };
        return parsed;
    }
    if (input["id"].get_type() != Variant::STRING) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Nearest item 'id' must be a String." };
        return parsed;
    }

    const String id = static_cast<String>(input["id"]);
    if (id.is_empty()) {
        parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                          "Nearest item requires a non-empty 'id'." };
        return parsed;
    }

    if (!input.has("embedding") ||
        input["embedding"].get_type() != Variant::ARRAY) {
        parsed.status = {
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "Nearest item requires 'embedding' as an Array of floats."
        };
        return parsed;
    }

    auto embedding = ParseEmbeddingArray(static_cast<Array>(input["embedding"]),
                                         "Nearest item 'embedding'",
                                         expected_dimension);
    if (!embedding.status.ok()) {
        parsed.status = std::move(embedding.status);
        return parsed;
    }

    parsed.item.id                 = GStrToCStr(id);
    parsed.item.embedding          = std::move(embedding.values);
    parsed.normalized["id"]        = id;
    parsed.normalized["embedding"] = embedding.normalized;

    if (input.has("text_payload") &&
        input["text_payload"].get_type() != Variant::NIL) {
        if (input["text_payload"].get_type() != Variant::STRING) {
            parsed.status = {
                PDJE_UTIL::common::StatusCode::invalid_argument,
                "Nearest item 'text_payload' must be a String when provided."
            };
            return parsed;
        }
        const String text_payload = static_cast<String>(input["text_payload"]);
        parsed.item.text_payload  = GStrToCStr(text_payload);
        parsed.normalized["text_payload"] = text_payload;
    }

    if (input.has("bytes_payload") &&
        input["bytes_payload"].get_type() != Variant::NIL) {
        if (input["bytes_payload"].get_type() != Variant::PACKED_BYTE_ARRAY) {
            parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                              "Nearest item 'bytes_payload' must be a "
                              "PackedByteArray when provided." };
            return parsed;
        }
        const PackedByteArray bytes_payload =
            static_cast<PackedByteArray>(input["bytes_payload"]);
        parsed.item.bytes_payload = PackedByteArrayToBytes(bytes_payload);
        parsed.normalized["bytes_payload"] = bytes_payload;
    }

    parsed.status = {};
    return parsed;
}

NearestSearchOptionsParseResult
ParseNearestSearchOptions(const Dictionary &input)
{
    NearestSearchOptionsParseResult parsed;
    parsed.normalized["limit"]    = static_cast<int64_t>(10);
    parsed.normalized["search_k"] = static_cast<int64_t>(-1);

    if (input.has("limit")) {
        int64_t limit = 0;
        if (!VariantToInteger(input["limit"], limit) || limit < 0) {
            parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                              "Nearest search option 'limit' must be a "
                              "non-negative integer when provided." };
            return parsed;
        }
        parsed.options.limit       = static_cast<size_t>(limit);
        parsed.normalized["limit"] = limit;
    }

    if (input.has("search_k")) {
        int64_t search_k = 0;
        if (!VariantToInteger(input["search_k"], search_k)) {
            parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                              "Nearest search option 'search_k' must be an "
                              "integer when provided." };
            return parsed;
        }
        parsed.options.search_k       = static_cast<int>(search_k);
        parsed.normalized["search_k"] = search_k;
    }

    parsed.status = {};
    return parsed;
}

bool
IsOpen(const std::unique_ptr<NearestState> &state)
{
    return state != nullptr && state->is_open;
}

Dictionary
MakeClosedResult(const std::unique_ptr<NearestState> &state,
                 const Dictionary                    &payload,
                 const Variant                       &id = Variant(),
                 const Variant &query_embedding          = Variant(),
                 const Variant &search_options           = Variant(),
                 const Variant &contains                 = Variant(),
                 const Variant &item                     = Variant(),
                 const Variant &hits                     = Variant(),
                 const Variant &keys                     = Variant())
{
    const PDJE_UTIL::common::Status status{
        PDJE_UTIL::common::StatusCode::closed, NEAREST_CLOSED_MESSAGE
    };
    return MakeStatusResult(
        false,
        status,
        MakeNearestData(payload,
                        state != nullptr ? state->root_path : String(),
                        state != nullptr ? state->config : Dictionary(),
                        false,
                        id,
                        query_embedding,
                        search_options,
                        contains,
                        item,
                        hits,
                        keys));
}

} // namespace

namespace godot::pdje_low_level_util::nearest {

Dictionary
Create(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
       const Dictionary                                            &config)
{
    auto parsed = ParseNearestConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeNearestData(MakeNearestSearchPayload(),
                                                String(),
                                                parsed.normalized,
                                                IsOpen(state),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant()));
    }

    auto created =
        pdje_low_level_util_internal::NativeNearestIndex::create(parsed.config);
    if (!created.ok()) {
        return MakeStatusResult(
            false,
            created.status(),
            MakeNearestData(MakeNearestSearchPayload(),
                            static_cast<String>(parsed.normalized["root_path"]),
                            parsed.normalized,
                            IsOpen(state),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant()));
    }

    return MakeSuccessResult(
        MakeNearestData(MakeNearestSearchPayload(),
                        static_cast<String>(parsed.normalized["root_path"]),
                        parsed.normalized,
                        IsOpen(state),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant()));
}

Dictionary
Destroy(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
        const Dictionary                                            &config)
{
    auto parsed = ParseNearestConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeNearestData(MakeNearestSearchPayload(),
                                                String(),
                                                parsed.normalized,
                                                IsOpen(state),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant()));
    }

    if (IsOpen(state) &&
        state->root_path ==
            static_cast<String>(parsed.normalized["root_path"])) {
        auto closed = state->index.close();
        if (!closed.ok()) {
            return MakeStatusResult(false,
                                    closed.status(),
                                    MakeNearestData(MakeNearestSearchPayload(),
                                                    state->root_path,
                                                    state->config,
                                                    true,
                                                    Variant(),
                                                    Variant(),
                                                    Variant(),
                                                    Variant(),
                                                    Variant(),
                                                    Variant(),
                                                    Variant()));
        }
        state->config    = Dictionary();
        state->root_path = String();
        state->dimension = 0;
        state->is_open   = false;
    }

    auto destroyed = pdje_low_level_util_internal::NativeNearestIndex::destroy(
        parsed.config);
    if (!destroyed.ok()) {
        return MakeStatusResult(
            false,
            destroyed.status(),
            MakeNearestData(MakeNearestSearchPayload(),
                            static_cast<String>(parsed.normalized["root_path"]),
                            parsed.normalized,
                            IsOpen(state),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant()));
    }

    return MakeSuccessResult(
        MakeNearestData(MakeNearestSearchPayload(),
                        static_cast<String>(parsed.normalized["root_path"]),
                        parsed.normalized,
                        IsOpen(state),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant()));
}

Dictionary
Open(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
     const Dictionary                                            &config)
{
    auto parsed = ParseNearestConfig(config);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeNearestData(MakeNearestSearchPayload(),
                                                String(),
                                                parsed.normalized,
                                                IsOpen(state),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant()));
    }

    if (IsOpen(state)) {
        const PDJE_UTIL::common::Status status{
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "Nearest database is already open. Close it before opening another "
            "one."
        };
        return MakeStatusResult(false,
                                status,
                                MakeNearestData(MakeNearestSearchPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant()));
    }

    if (state == nullptr) {
        state = std::make_unique<NearestState>();
    }

    auto opened =
        pdje_low_level_util_internal::NativeNearestIndex::open(parsed.config);
    if (!opened.ok()) {
        return MakeStatusResult(
            false,
            opened.status(),
            MakeNearestData(MakeNearestSearchPayload(),
                            static_cast<String>(parsed.normalized["root_path"]),
                            parsed.normalized,
                            false,
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant(),
                            Variant()));
    }

    state->index     = std::move(opened.value());
    state->config    = parsed.normalized;
    state->root_path = static_cast<String>(parsed.normalized["root_path"]);
    state->dimension = static_cast<int64_t>(parsed.normalized["dimension"]);
    state->is_open   = true;

    return MakeSuccessResult(MakeNearestData(MakeNearestSearchPayload(),
                                             state->root_path,
                                             state->config,
                                             true,
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant()));
}

Dictionary
Close(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state, MakeNearestSearchPayload());
    }

    auto closed = state->index.close();
    if (!closed.ok()) {
        return MakeStatusResult(false,
                                closed.status(),
                                MakeNearestData(MakeNearestSearchPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant()));
    }

    Dictionary config    = state->config;
    String     root_path = state->root_path;
    state->config        = Dictionary();
    state->root_path     = String();
    state->dimension     = 0;
    state->is_open       = false;

    return MakeSuccessResult(MakeNearestData(MakeNearestSearchPayload(),
                                             root_path,
                                             config,
                                             false,
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant()));
}

Dictionary
Contains(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
         const String                                                &id)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(
            state, MakeNearestSearchPayload(), id, Variant(), Variant(), false);
    }

    auto contains = state->index.contains(GStrToCStr(id));
    if (!contains.ok()) {
        return MakeStatusResult(false,
                                contains.status(),
                                MakeNearestData(MakeNearestSearchPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                id,
                                                Variant(),
                                                Variant(),
                                                false,
                                                Variant(),
                                                Variant(),
                                                Variant()));
    }

    return MakeSuccessResult(MakeNearestData(MakeNearestSearchPayload(),
                                             state->root_path,
                                             state->config,
                                             true,
                                             id,
                                             Variant(),
                                             Variant(),
                                             contains.value(),
                                             Variant(),
                                             Variant(),
                                             Variant()));
}

Dictionary
GetItem(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
        const String                                                &id)
{
    const Variant empty_item = MakeNearestItemPayload()["item"];
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                MakeNearestItemPayload(),
                                id,
                                Variant(),
                                Variant(),
                                Variant(),
                                empty_item);
    }

    auto item = state->index.get_item(GStrToCStr(id));
    if (!item.ok()) {
        return MakeStatusResult(false,
                                item.status(),
                                MakeNearestData(MakeNearestItemPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                id,
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                empty_item,
                                                Variant(),
                                                Variant()));
    }

    return MakeSuccessResult(
        MakeNearestData(MakeNearestItemPayload(),
                        state->root_path,
                        state->config,
                        true,
                        id,
                        Variant(),
                        Variant(),
                        Variant(),
                        MakeNearestItemDictionary(item.value()),
                        Variant(),
                        Variant()));
}

Dictionary
UpsertItem(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
           const Dictionary                                            &item)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                MakeNearestItemPayload(),
                                Variant(),
                                Variant(),
                                Variant(),
                                Variant(),
                                item);
    }

    auto parsed = ParseNearestItem(item, state->dimension);
    if (!parsed.status.ok()) {
        return MakeStatusResult(false,
                                parsed.status,
                                MakeNearestData(MakeNearestItemPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                parsed.normalized["id"],
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                parsed.normalized,
                                                Variant(),
                                                Variant()));
    }

    auto upserted = state->index.upsert_item(parsed.item);
    if (!upserted.ok()) {
        return MakeStatusResult(false,
                                upserted.status(),
                                MakeNearestData(MakeNearestItemPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                parsed.normalized["id"],
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                parsed.normalized,
                                                Variant(),
                                                Variant()));
    }

    return MakeSuccessResult(MakeNearestData(MakeNearestItemPayload(),
                                             state->root_path,
                                             state->config,
                                             true,
                                             parsed.normalized["id"],
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             parsed.normalized,
                                             Variant(),
                                             Variant()));
}

Dictionary
EraseItem(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
          const String                                                &id)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state, MakeNearestSearchPayload(), id);
    }

    auto erased = state->index.erase_item(GStrToCStr(id));
    if (!erased.ok()) {
        return MakeStatusResult(false,
                                erased.status(),
                                MakeNearestData(MakeNearestSearchPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                id,
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant()));
    }

    return MakeSuccessResult(MakeNearestData(MakeNearestSearchPayload(),
                                             state->root_path,
                                             state->config,
                                             true,
                                             id,
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant(),
                                             Variant()));
}

Dictionary
Search(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
       const Array      &query_embedding,
       const Dictionary &options)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                MakeNearestSearchPayload(),
                                Variant(),
                                query_embedding,
                                options,
                                Variant(),
                                Variant(),
                                Array());
    }

    auto parsed_query = ParseEmbeddingArray(
        query_embedding, "Nearest search 'query_embedding'", state->dimension);
    if (!parsed_query.status.ok()) {
        return MakeStatusResult(false,
                                parsed_query.status,
                                MakeNearestData(MakeNearestSearchPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                Variant(),
                                                parsed_query.normalized,
                                                options,
                                                Variant(),
                                                Variant(),
                                                Array(),
                                                Variant()));
    }

    auto parsed_options = ParseNearestSearchOptions(options);
    if (!parsed_options.status.ok()) {
        return MakeStatusResult(false,
                                parsed_options.status,
                                MakeNearestData(MakeNearestSearchPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                Variant(),
                                                parsed_query.normalized,
                                                parsed_options.normalized,
                                                Variant(),
                                                Variant(),
                                                Array(),
                                                Variant()));
    }

    auto hits =
        state->index.search(std::span<const float>(parsed_query.values.data(),
                                                   parsed_query.values.size()),
                            parsed_options.options);
    if (!hits.ok()) {
        return MakeStatusResult(false,
                                hits.status(),
                                MakeNearestData(MakeNearestSearchPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                Variant(),
                                                parsed_query.normalized,
                                                parsed_options.normalized,
                                                Variant(),
                                                Variant(),
                                                Array(),
                                                Variant()));
    }

    Array normalized_hits;
    for (const auto &hit : hits.value()) {
        normalized_hits.append(MakeNearestHitDictionary(hit));
    }

    return MakeSuccessResult(MakeNearestData(MakeNearestSearchPayload(),
                                             state->root_path,
                                             state->config,
                                             true,
                                             Variant(),
                                             parsed_query.normalized,
                                             parsed_options.normalized,
                                             Variant(),
                                             Variant(),
                                             normalized_hits,
                                             Variant()));
}

Dictionary
ListKeys(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state)
{
    if (!IsOpen(state)) {
        return MakeClosedResult(state,
                                MakeNearestSearchPayload(),
                                Variant(),
                                Variant(),
                                Variant(),
                                Variant(),
                                Variant(),
                                Variant(),
                                PackedStringArray());
    }

    auto keys = state->index.list_keys();
    if (!keys.ok()) {
        return MakeStatusResult(false,
                                keys.status(),
                                MakeNearestData(MakeNearestSearchPayload(),
                                                state->root_path,
                                                state->config,
                                                true,
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                Variant(),
                                                PackedStringArray()));
    }

    return MakeSuccessResult(
        MakeNearestData(MakeNearestSearchPayload(),
                        state->root_path,
                        state->config,
                        true,
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        Variant(),
                        KeysToPackedStringArray(keys.value())));
}

void
Shutdown(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state)
{
    if (IsOpen(state)) {
        (void)state->index.close();
    }
}

} // namespace godot::pdje_low_level_util::nearest
