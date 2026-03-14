#pragma once

#include "util/db/backends/SqliteBackend.hpp"
#include "util/db/relational/Database.hpp"

#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

#include <memory>

namespace godot {
namespace pdje_low_level_util_internal {
using NativeRelationalDatabase = PDJE_UTIL::db::relational::RelationalDatabase<
    PDJE_UTIL::db::backends::SqliteBackend>;

struct RelationalState {
    NativeRelationalDatabase database;
    Dictionary               config;
    String                   path;
    bool                     is_open = false;
};
} // namespace pdje_low_level_util_internal
} // namespace godot

namespace godot::pdje_low_level_util::relational {

Dictionary
Create(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
       const Dictionary                                               &config);

Dictionary
Destroy(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
        const Dictionary                                               &config);

Dictionary
Open(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
     const Dictionary                                               &config);

Dictionary
Close(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state);

Dictionary
Execute(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
        const String                                                   &sql,
        const Array                                                    &params);

Dictionary
Query(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state,
      const String                                                   &sql,
      const Array                                                    &params);

Dictionary
BeginTransaction(
    std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state);

Dictionary
Commit(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state);

Dictionary
Rollback(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state);

void
Shutdown(std::unique_ptr<pdje_low_level_util_internal::RelationalState> &state);

} // namespace godot::pdje_low_level_util::relational
