#pragma once

#include "util/db/backends/AnnoyBackend.hpp"
#include "util/db/nearest/Index.hpp"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cstdint>
#include <memory>

namespace godot {
namespace pdje_low_level_util_internal {
using NativeNearestIndex = PDJE_UTIL::db::nearest::NearestNeighborIndex<
    PDJE_UTIL::db::backends::AnnoyBackend>;

struct NearestState {
    NativeNearestIndex index;
    Dictionary         config;
    String             root_path;
    int64_t            dimension = 0;
    bool               is_open   = false;
};
} // namespace pdje_low_level_util_internal
} // namespace godot

namespace godot::pdje_low_level_util::nearest {

Dictionary
Create(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
       const Dictionary                                            &config);

Dictionary
Destroy(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
        const Dictionary                                            &config);

Dictionary
Open(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
     const Dictionary                                            &config);

Dictionary
Close(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state);

Dictionary
Contains(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
         const String                                                &id);

Dictionary
GetItem(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
        const String                                                &id);

Dictionary
UpsertItem(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
           const Dictionary                                            &item);

Dictionary
EraseItem(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
          const String                                                &id);

Dictionary
Search(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state,
       const Array      &query_embedding,
       const Dictionary &options);

Dictionary
ListKeys(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state);

void
Shutdown(std::unique_ptr<pdje_low_level_util_internal::NearestState> &state);

} // namespace godot::pdje_low_level_util::nearest
