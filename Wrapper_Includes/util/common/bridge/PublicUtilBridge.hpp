#pragma once

#include "PDJE_Core_Wrapper.hpp"
#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/db/keyvalue/PDJE_KeyValueDB.hpp"

#include <godot_cpp/core/print_string.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cstdint>
#include <exception>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

namespace godot::pdje_public_util::common {

inline void
print_method_error(const char *method_name, const String &detail)
{
    print_error(String(method_name) + " failed: " + detail);
}

inline String
exception_detail(const std::exception &exception)
{
    return CStrToGStr(exception.what());
}

template <typename Callable>
bool
call_or_error(const char *method_name, Callable &&callable) noexcept
{
    try {
        std::invoke(std::forward<Callable>(callable));
        return true;
    } catch (const std::exception &exception) {
        print_method_error(method_name, exception_detail(exception));
    } catch (...) {
        print_method_error(method_name, "unknown native Util error");
    }
    return false;
}

template <typename Callable>
auto
value_or_error(const char *method_name, Callable &&callable) noexcept
    -> std::optional<std::invoke_result_t<Callable>>
{
    try {
        return std::invoke(std::forward<Callable>(callable));
    } catch (const std::exception &exception) {
        print_method_error(method_name, exception_detail(exception));
    } catch (...) {
        print_method_error(method_name, "unknown native Util error");
    }
    return std::nullopt;
}

inline bool
variant_to_packed_float32_array(const Variant      &value,
                                PackedFloat32Array &out_values)
{
    if (value.get_type() == Variant::PACKED_FLOAT32_ARRAY) {
        out_values = static_cast<PackedFloat32Array>(value);
        return true;
    }

    if (value.get_type() != Variant::ARRAY) {
        return false;
    }

    const Array source = static_cast<Array>(value);
    out_values.resize(source.size());
    for (int64_t i = 0; i < source.size(); ++i) {
        float parsed = 0.0f;
        if (!pdje_low_level_util::common::VariantToFloat(source[i], parsed)) {
            out_values = PackedFloat32Array();
            return false;
        }
        out_values.set(i, parsed);
    }

    return true;
}

inline bool
variant_to_packed_string_array(const Variant     &value,
                               PackedStringArray &out_values)
{
    if (value.get_type() == Variant::PACKED_STRING_ARRAY) {
        out_values = static_cast<PackedStringArray>(value);
        return true;
    }

    if (value.get_type() != Variant::ARRAY) {
        return false;
    }

    const Array source = static_cast<Array>(value);
    out_values         = PackedStringArray();
    for (int64_t i = 0; i < source.size(); ++i) {
        if (source[i].get_type() != Variant::STRING) {
            out_values = PackedStringArray();
            return false;
        }
        out_values.append(static_cast<String>(source[i]));
    }

    return true;
}

inline bool
CheckDB(PDJE_KeyValueDB *kvp)
{
    if (kvp == nullptr) {
        return false;
    } else if (!kvp->IsOpen()) {
        return false;
    }
    return true;
}
inline void
BuildCacheSourceKey(const String &musicTitle,
                    const String &composer,
                    const float  &bpm,
                    String       &out_source_key)
{
    out_source_key = String("query|title:") + musicTitle +
                     String("|composer:") + composer + String("|bpm:") +
                     String::num_real(std::floor(bpm));
}
} // namespace godot::pdje_public_util::common
