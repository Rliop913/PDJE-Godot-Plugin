#pragma once

#include "util/db/keyvalue/adapter/LowLevelKeyValueAdapter.hpp"

#include "util/db/backends/RocksDbBackend.hpp"

namespace godot::pdje_low_level_util_internal {

using NativeKeyValueBackend = PDJE_UTIL::db::backends::RocksDbBackend;

struct KeyValueState {
    NativeKeyValueBackend backend;
    Dictionary            config;
    String                path;
    bool                  is_open = false;
};

} // namespace godot::pdje_low_level_util_internal
