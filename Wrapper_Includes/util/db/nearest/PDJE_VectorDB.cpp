#include "util/db/nearest/PDJE_VectorDB.hpp"

#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"
#include "util/db/backends/AnnoyBackend.hpp"
#include "util/db/nearest/Index.hpp"
#include "util/db/nearest/PDJE_VectorTypes.hpp"

#include <godot_cpp/core/class_db.hpp>

#include <cstdint>
#include <cstring>
#include <memory>
#include <span>
#include <utility>
#include <vector>

using namespace godot;

namespace godot::pdje_vector_db_internal {

using NativeNearestIndex = PDJE_UTIL::db::nearest::NearestNeighborIndex<
    PDJE_UTIL::db::backends::AnnoyBackend>;

struct State {
    NativeNearestIndex index;
    String             root_path;
    int64_t            dimension = 0;
};

} // namespace godot::pdje_vector_db_internal

namespace {

using godot::pdje_low_level_util::common::BytesToPackedByteArray;
using godot::pdje_low_level_util::common::KeysToPackedStringArray;
using godot::pdje_low_level_util::common::PackedByteArrayToBytes;
using godot::pdje_public_util::common::call_or_error;
using godot::pdje_public_util::common::print_method_error;
using godot::pdje_public_util::common::value_or_error;
using godot::pdje_vector_db_internal::NativeNearestIndex;
using godot::pdje_vector_db_internal::State;

bool
ValidateConfig(const char *method_name,
               const String &root_path,
               int           dimension,
               int           trees)
{
    if (root_path.is_empty()) {
        print_method_error(method_name, "'root_path' must not be empty");
        return false;
    }
    if (dimension <= 0) {
        print_method_error(method_name, "'dimension' must be positive");
        return false;
    }
    if (trees <= 0) {
        print_method_error(method_name, "'trees' must be positive");
        return false;
    }
    return true;
}

bool
RequireOpen(const char *method_name, const std::unique_ptr<State> &state)
{
    if (state != nullptr && state->index.is_open) {
        return true;
    }

    print_method_error(method_name, "Vector database is not open");
    return false;
}

PDJE_UTIL::db::backends::AnnoyConfig
MakeConfig(const String &root_path,
           int           dimension,
           int           trees,
           bool          prefault,
           bool          create_if_missing,
           bool          truncate_if_exists,
           bool          read_only)
{
    return { GpathToCPath(root_path),
             { create_if_missing, truncate_if_exists, read_only },
             static_cast<std::size_t>(dimension),
             trees,
             prefault };
}

void
ResetState(State &state)
{
    state.root_path = String();
    state.dimension = 0;
}

bool
CloseState(const char *method_name, const std::unique_ptr<State> &state)
{
    if (state == nullptr || !state->index.is_open) {
        return true;
    }

    if (!call_or_error(method_name, [&]() { state->index.close(); })) {
        return false;
    }

    ResetState(*state);
    return true;
}

PackedFloat32Array
ToPackedFloat32Array(const std::vector<float> &values)
{
    PackedFloat32Array packed;
    packed.resize(static_cast<int64_t>(values.size()));
    if (!values.empty()) {
        std::memcpy(
            packed.ptrw(), values.data(), values.size() * sizeof(float));
    }
    return packed;
}

std::vector<float>
ToFloatVector(const PackedFloat32Array &values)
{
    std::vector<float> out(static_cast<std::size_t>(values.size()));
    for (int64_t i = 0; i < values.size(); ++i) {
        out[static_cast<std::size_t>(i)] = values[i];
    }
    return out;
}

Ref<PDJE_VectorItem>
MakeVectorItemRef(const PDJE_UTIL::db::nearest::Item &item)
{
    Ref<PDJE_VectorItem> ref;
    ref.instantiate();
    ref->set_id(CStrToGStr(item.id));
    ref->set_embedding(ToPackedFloat32Array(item.embedding));
    ref->set_text_payload(item.text_payload.has_value()
                              ? CStrToGStr(*item.text_payload)
                              : String());
    ref->set_bytes_payload(item.bytes_payload.has_value()
                               ? BytesToPackedByteArray(*item.bytes_payload)
                               : PackedByteArray());
    return ref;
}

Ref<PDJE_VectorHit>
MakeVectorHitRef(const PDJE_UTIL::db::nearest::SearchHit &hit)
{
    Ref<PDJE_VectorHit> ref;
    ref.instantiate();
    ref->set_id(CStrToGStr(hit.id));
    ref->set_distance(hit.distance);
    ref->set_text_payload(hit.text_payload.has_value()
                              ? CStrToGStr(*hit.text_payload)
                              : String());
    ref->set_bytes_payload(hit.bytes_payload.has_value()
                               ? BytesToPackedByteArray(*hit.bytes_payload)
                               : PackedByteArray());
    return ref;
}

PDJE_UTIL::db::nearest::Item
MakeNearestItem(const Ref<PDJE_VectorItem> &item)
{
    PDJE_UTIL::db::nearest::Item native_item;
    native_item.id        = GStrToCStr(item->get_id());
    native_item.embedding = ToFloatVector(item->get_embedding());

    const String text_payload = item->get_text_payload();
    if (!text_payload.is_empty()) {
        native_item.text_payload = GStrToCStr(text_payload);
    }

    const PackedByteArray bytes_payload = item->get_bytes_payload();
    if (!bytes_payload.is_empty()) {
        native_item.bytes_payload = PackedByteArrayToBytes(bytes_payload);
    }

    return native_item;
}

} // namespace

void
PDJE_VectorDB::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("Create",
                                  "root_path",
                                  "dimension",
                                  "trees",
                                  "prefault",
                                  "truncate_if_exists"),
                         &PDJE_VectorDB::Create,
                         DEFVAL(10),
                         DEFVAL(false),
                         DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Destroy",
                                  "root_path",
                                  "dimension",
                                  "trees",
                                  "prefault"),
                         &PDJE_VectorDB::Destroy,
                         DEFVAL(10),
                         DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Open",
                                  "root_path",
                                  "dimension",
                                  "trees",
                                  "prefault",
                                  "create_if_missing",
                                  "truncate_if_exists",
                                  "read_only"),
                         &PDJE_VectorDB::Open,
                         DEFVAL(10),
                         DEFVAL(false),
                         DEFVAL(false),
                         DEFVAL(false),
                         DEFVAL(false));
    ClassDB::bind_method(D_METHOD("Close"), &PDJE_VectorDB::Close);
    ClassDB::bind_method(D_METHOD("IsOpen"), &PDJE_VectorDB::IsOpen);
    ClassDB::bind_method(D_METHOD("GetRootPath"), &PDJE_VectorDB::GetRootPath);
    ClassDB::bind_method(D_METHOD("GetDimension"), &PDJE_VectorDB::GetDimension);
    ClassDB::bind_method(D_METHOD("Contains", "id"), &PDJE_VectorDB::Contains);
    ClassDB::bind_method(D_METHOD("GetItem", "id"), &PDJE_VectorDB::GetItem);
    ClassDB::bind_method(D_METHOD("UpsertItem", "item"),
                         &PDJE_VectorDB::UpsertItem);
    ClassDB::bind_method(D_METHOD("EraseItem", "id"),
                         &PDJE_VectorDB::EraseItem);
    ClassDB::bind_method(D_METHOD("Search",
                                  "query_embedding",
                                  "limit",
                                  "search_k"),
                         &PDJE_VectorDB::Search,
                         DEFVAL(10),
                         DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("ListKeys"), &PDJE_VectorDB::ListKeys);
}

bool
PDJE_VectorDB::Create(String root_path,
                      int    dimension,
                      int    trees,
                      bool   prefault,
                      bool   truncate_if_exists)
{
    if (!ValidateConfig("PDJE_VectorDB.Create", root_path, dimension, trees)) {
        return false;
    }

    const auto config = MakeConfig(
        root_path, dimension, trees, prefault, false, false, false);
    return call_or_error("PDJE_VectorDB.Create", [&]() {
        if (truncate_if_exists) {
            NativeNearestIndex::destroy(config);
        }
        NativeNearestIndex::create(config);
    });
}

bool
PDJE_VectorDB::Destroy(String root_path, int dimension, int trees, bool prefault)
{
    if (!ValidateConfig("PDJE_VectorDB.Destroy", root_path, dimension, trees)) {
        return false;
    }

    if (state_ != nullptr && state_->index.is_open &&
        state_->root_path == root_path &&
        !CloseState("PDJE_VectorDB.Destroy", state_)) {
        return false;
    }

    return call_or_error("PDJE_VectorDB.Destroy", [&]() {
        NativeNearestIndex::destroy(MakeConfig(root_path,
                                               dimension,
                                               trees,
                                               prefault,
                                               false,
                                               false,
                                               false));
    });
}

bool
PDJE_VectorDB::Open(String root_path,
                    int    dimension,
                    int    trees,
                    bool   prefault,
                    bool   create_if_missing,
                    bool   truncate_if_exists,
                    bool   read_only)
{
    if (!ValidateConfig("PDJE_VectorDB.Open", root_path, dimension, trees)) {
        return false;
    }
    if (state_ != nullptr && state_->index.is_open) {
        print_method_error("PDJE_VectorDB.Open",
                           "Vector database is already open");
        return false;
    }

    if (state_ == nullptr) {
        state_ = std::make_unique<State>();
    }

    auto opened = value_or_error("PDJE_VectorDB.Open", [&]() {
        return NativeNearestIndex::open(MakeConfig(root_path,
                                                   dimension,
                                                   trees,
                                                   prefault,
                                                   create_if_missing,
                                                   truncate_if_exists,
                                                   read_only));
    });
    if (!opened.has_value()) {
        return false;
    }

    state_->index     = std::move(*opened);
    state_->root_path = root_path;
    state_->dimension = dimension;
    return true;
}

bool
PDJE_VectorDB::Close()
{
    if (!RequireOpen("PDJE_VectorDB.Close", state_)) {
        return false;
    }
    return CloseState("PDJE_VectorDB.Close", state_);
}

bool
PDJE_VectorDB::IsOpen() const
{
    return state_ != nullptr && state_->index.is_open;
}

String
PDJE_VectorDB::GetRootPath() const
{
    return state_ != nullptr ? state_->root_path : String();
}

int
PDJE_VectorDB::GetDimension() const
{
    return state_ != nullptr ? static_cast<int>(state_->dimension) : 0;
}

bool
PDJE_VectorDB::Contains(String id)
{
    if (!RequireOpen("PDJE_VectorDB.Contains", state_)) {
        return false;
    }

    auto contains = value_or_error("PDJE_VectorDB.Contains", [&]() {
        return state_->index.contains(GStrToCStr(id));
    });
    return contains.value_or(false);
}

Ref<PDJE_VectorItem>
PDJE_VectorDB::GetItem(String id)
{
    if (!RequireOpen("PDJE_VectorDB.GetItem", state_)) {
        return Ref<PDJE_VectorItem>();
    }

    auto item = value_or_error("PDJE_VectorDB.GetItem", [&]() {
        return state_->index.get_item(GStrToCStr(id));
    });
    return item.has_value() ? MakeVectorItemRef(*item)
                            : Ref<PDJE_VectorItem>();
}

bool
PDJE_VectorDB::UpsertItem(Ref<PDJE_VectorItem> item)
{
    if (!RequireOpen("PDJE_VectorDB.UpsertItem", state_)) {
        return false;
    }
    if (item.is_null()) {
        print_method_error("PDJE_VectorDB.UpsertItem", "item must not be null");
        return false;
    }
    if (item->get_embedding().size() != state_->dimension) {
        print_method_error("PDJE_VectorDB.UpsertItem",
                           "item embedding size must match the configured dimension");
        return false;
    }

    return call_or_error("PDJE_VectorDB.UpsertItem", [&]() {
        state_->index.upsert_item(MakeNearestItem(item));
    });
}

bool
PDJE_VectorDB::EraseItem(String id)
{
    if (!RequireOpen("PDJE_VectorDB.EraseItem", state_)) {
        return false;
    }

    return call_or_error("PDJE_VectorDB.EraseItem", [&]() {
        state_->index.erase_item(GStrToCStr(id));
    });
}

Array
PDJE_VectorDB::Search(PackedFloat32Array query_embedding, int limit, int search_k)
{
    if (!RequireOpen("PDJE_VectorDB.Search", state_)) {
        return Array();
    }
    if (limit < 0) {
        print_method_error("PDJE_VectorDB.Search",
                           "'limit' must be non-negative");
        return Array();
    }
    if (query_embedding.size() != state_->dimension) {
        print_method_error("PDJE_VectorDB.Search",
                           "query_embedding size must match the configured dimension");
        return Array();
    }

    const auto query_values = ToFloatVector(query_embedding);
    auto hits = value_or_error("PDJE_VectorDB.Search", [&]() {
        return state_->index.search(
            std::span<const float>(query_values.data(), query_values.size()),
            { static_cast<std::size_t>(limit), search_k });
    });
    if (!hits.has_value()) {
        return Array();
    }

    Array out;
    for (const auto &hit : *hits) {
        out.append(MakeVectorHitRef(hit));
    }
    return out;
}

PackedStringArray
PDJE_VectorDB::ListKeys()
{
    if (!RequireOpen("PDJE_VectorDB.ListKeys", state_)) {
        return PackedStringArray();
    }

    auto keys = value_or_error("PDJE_VectorDB.ListKeys",
                               [&]() { return state_->index.list_keys(); });
    return keys.has_value() ? KeysToPackedStringArray(*keys)
                            : PackedStringArray();
}

PDJE_VectorDB::PDJE_VectorDB() = default;

PDJE_VectorDB::~PDJE_VectorDB()
{
    (void)CloseState("PDJE_VectorDB.~PDJE_VectorDB", state_);
}

void
PDJE_VectorDB::_ready()
{
}
