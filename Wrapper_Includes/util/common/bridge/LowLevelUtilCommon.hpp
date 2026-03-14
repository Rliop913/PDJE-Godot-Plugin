#pragma once

#include "PDJE_Utils.hpp"

#include "util/common/Status.hpp"
#include "util/common/StatusCode.hpp"
#include "util/db/DbTypes.hpp"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

namespace godot::pdje_low_level_util::common {

inline String
StatusCodeToGodotCode(PDJE_UTIL::common::StatusCode code)
{
    using PDJE_UTIL::common::StatusCode;

    switch (code) {
    case StatusCode::ok:
        return "OK";
    case StatusCode::invalid_argument:
        return "INVALID_ARGUMENT";
    case StatusCode::not_found:
        return "NOT_FOUND";
    case StatusCode::type_mismatch:
        return "TYPE_MISMATCH";
    case StatusCode::unsupported:
        return "UNSUPPORTED";
    case StatusCode::io_error:
        return "IO_ERROR";
    case StatusCode::closed:
        return "CLOSED";
    case StatusCode::backend_error:
        return "BACKEND_ERROR";
    case StatusCode::out_of_range:
        return "OUT_OF_RANGE";
    case StatusCode::internal_error:
        return "INTERNAL_ERROR";
    }

    return "INTERNAL_ERROR";
}

inline String
StatusMessageToGodot(const PDJE_UTIL::common::Status &status)
{
    if (status.message.empty()) {
        return String();
    }

    return CStrToGStr(status.message);
}

inline Dictionary
MakeResult(bool           ok,
           const String  &code,
           const String  &message,
           const Variant &data = Variant(nullptr))
{
    Dictionary result;
    result["ok"]      = ok;
    result["code"]    = code;
    result["message"] = message;
    result["data"]    = data;
    return result;
}

inline Dictionary
MakeStatusResult(bool                             ok,
                 const PDJE_UTIL::common::Status &status,
                 const Variant                   &data = Variant(nullptr))
{
    return MakeResult(ok,
                      StatusCodeToGodotCode(status.code),
                      StatusMessageToGodot(status),
                      data);
}

inline Dictionary
MakeSuccessResult(const Variant &data = Variant(nullptr))
{
    return MakeResult(true, "OK", String(), data);
}

inline Dictionary
MakeNotImplementedResult(const String  &method_name,
                         const Variant &data = Variant(nullptr))
{
    return MakeResult(
        false,
        "NOT_IMPLEMENTED",
        method_name +
            String(" is a contract placeholder and has no backend yet."),
        data);
}

inline bool
DictionaryBoolAt(const Dictionary &dict, const char *key, bool default_value)
{
    if (!dict.has(key)) {
        return default_value;
    }

    return static_cast<bool>(dict[key]);
}

inline PackedByteArray
BytesToPackedByteArray(const std::vector<std::byte> &bytes)
{
    PackedByteArray packed;
    packed.resize(static_cast<int64_t>(bytes.size()));
    for (int64_t i = 0; i < static_cast<int64_t>(bytes.size()); ++i) {
        packed.set(i, static_cast<uint8_t>(bytes[static_cast<size_t>(i)]));
    }
    return packed;
}

inline std::vector<std::byte>
PackedByteArrayToBytes(const PackedByteArray &value)
{
    std::vector<std::byte> bytes;
    bytes.resize(static_cast<size_t>(value.size()));
    for (int64_t i = 0; i < value.size(); ++i) {
        bytes[static_cast<size_t>(i)] = std::byte(value[i]);
    }
    return bytes;
}

inline PackedStringArray
KeysToPackedStringArray(const std::vector<PDJE_UTIL::db::Key> &keys)
{
    PackedStringArray packed_keys;
    for (const auto &key : keys) {
        packed_keys.append(CStrToGStr(key));
    }
    return packed_keys;
}

inline bool
VariantToFloat(const Variant &value, float &out_value)
{
    switch (value.get_type()) {
    case Variant::INT:
        out_value = static_cast<float>(static_cast<int64_t>(value));
        return true;
    case Variant::FLOAT:
        out_value = static_cast<float>(static_cast<double>(value));
        return true;
    default:
        return false;
    }
}

inline bool
VariantToInteger(const Variant &value, int64_t &out_value)
{
    switch (value.get_type()) {
    case Variant::INT:
        out_value = static_cast<int64_t>(value);
        return true;
    case Variant::FLOAT: {
        const double number = static_cast<double>(value);
        if (!std::isfinite(number) || std::floor(number) != number) {
            return false;
        }
        out_value = static_cast<int64_t>(number);
        return true;
    }
    default:
        return false;
    }
}

inline bool
VariantToBool(const Variant &value, bool &out_value)
{
    if (value.get_type() != Variant::BOOL) {
        return false;
    }

    out_value = static_cast<bool>(value);
    return true;
}

} // namespace godot::pdje_low_level_util::common
