#pragma once

#include "util/db/backends/RocksDbBackend.hpp"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <memory>

namespace godot {
namespace pdje_low_level_util_internal {
using NativeKeyValueBackend = PDJE_UTIL::db::backends::RocksDbBackend;

struct KeyValueState {
    NativeKeyValueBackend backend;
    Dictionary            config;
    String                path;
    bool                  is_open = false;
};
} // namespace pdje_low_level_util_internal
} // namespace godot

namespace godot::pdje_low_level_util::keyvalue {

Dictionary
Create(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
       const Dictionary                                             &config);

Dictionary
Destroy(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
        const Dictionary                                             &config);

Dictionary
Open(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
     const Dictionary                                             &config);

Dictionary
Close(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state);

Dictionary
Contains(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
         const String                                                 &key);

Dictionary
GetText(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
        const String                                                 &key);

Dictionary
GetBytes(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
         const String                                                 &key);

Dictionary
PutText(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
        const String                                                 &key,
        const String                                                 &value);

Dictionary
PutBytes(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
         const String                                                 &key,
         const PackedByteArray                                        &value);

Dictionary
Erase(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
      const String                                                 &key);

Dictionary
ListKeys(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state,
         const String                                                 &prefix);

void
Shutdown(std::unique_ptr<pdje_low_level_util_internal::KeyValueState> &state);

} // namespace godot::pdje_low_level_util::keyvalue
