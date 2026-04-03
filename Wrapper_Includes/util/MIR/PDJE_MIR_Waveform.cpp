#include "WaveformWebp.hpp"
#include "pdje_util_common.hpp"
#include "util/MIR/PDJE_MIR.hpp"

#include "PDJE_Core_Wrapper.hpp"
#include "util/MIR/LowLevelWaveformAdapter.hpp"
#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"
#include "util/db/keyvalue/PDJE_KeyValueDB.hpp"

#include <algorithm>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cmath>
#include <cstdint>
#include <mutex>
#include <utility>

using namespace godot;

namespace {

using godot::pdje_low_level_util::common::StatusCodeToGodotCode;
using godot::pdje_low_level_util::common::StatusMessageToGodot;
using godot::pdje_public_util::common::print_method_error;

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
PrintStatusError(const char                      *method_name,
                 const PDJE_UTIL::common::Status &status)
{
    print_method_error(method_name, FormatStatusDetail(status));
}

String
BuildCacheKey(const String &cache_source_key,
              int           width,
              int           height,
              int           pcm_per_pixel)
{
    return String("MUSR|") + cache_source_key + String("|w:") +
           String::num_int64(width) + String("|h:") +
           String::num_int64(height) + String("|pp:") +
           String::num_int64(pcm_per_pixel);
}

Array
SliceImages(const Array &images, int start_index, int end_index)
{
    Array sliced;
    if (images.is_empty()) {
        return sliced;
    }
    const int max_index = static_cast<int>(images.size()) - 1;
    start_index         = std::clamp(start_index, 0, max_index);
    end_index           = end_index < 0 ? max_index : end_index;
    end_index           = end_index > max_index ? max_index : end_index;

    if (start_index > end_index) {
        std::swap(start_index, end_index);
    }

    for (int i = start_index; i <= end_index; ++i) {
        sliced.append(images[i]);
    }
    return sliced;
}

bool
DecodeImageArray(const PackedByteArray &blob, Array &images)
{
    const Variant decoded = UtilityFunctions::bytes_to_var(blob);
    if (decoded.get_type() != Variant::ARRAY) {
        return false;
    }

    const Array decoded_array = static_cast<Array>(decoded);
    for (int64_t i = 0; i < decoded_array.size(); ++i) {
        if (decoded_array[i].get_type() != Variant::PACKED_BYTE_ARRAY) {
            return false;
        }
    }

    images = decoded_array;
    return true;
}

} // namespace

Array
PDJE_MIR::SoundToWaveform(PDJE_Wrapper    *core_api,
                          PDJE_KeyValueDB *cache_db,
                          String           musicTitle,
                          String           composer,
                          float            bpm,
                          int              pcm_per_pixel,
                          int              width,
                          int              height,
                          int              start_index,
                          int              end_index)
{
    Array out;

    if (core_api == nullptr) {
        print_method_error("PDJE_MIR.SoundToWaveform", "core_api is null");
        return out;
    }
    if (!core_api->engine.has_value()) {
        print_method_error("PDJE_MIR.SoundToWaveform", "core_api is null");
        return {};
    }
    if (pcm_per_pixel <= 0 || width <= 0 || height <= 0) {
        print_method_error("PDJE_MIR.SoundToWaveform",
                           "width, height, and pcm_per_pixel must be positive");
        return out;
    }
    auto musSearched = core_api->engine->SearchMusic(
        GStrToCStr(musicTitle), GStrToCStr(composer), bpm);
    if (musSearched.empty()) {
        return {};
    }
    String cache_source_key;
    pdje_public_util::common::BuildCacheSourceKey(
        musicTitle, composer, musSearched.front().bpm, cache_source_key);

    bool can_use_cache = pdje_public_util::common::CheckDB(cache_db);

    const String cache_key =
        BuildCacheKey(cache_source_key, width, height, pcm_per_pixel);
    if (can_use_cache) {
        PackedByteArray cached_blob;
        bool            cached_found = false;
        if (cache_db->TryGetBytesSilently(
                cache_key, cached_blob, cached_found) &&
            cached_found) {
            Array cached_images;
            if (DecodeImageArray(cached_blob, cached_images)) {
                return SliceImages(cached_images, start_index, end_index);
            }
        }
    }

    auto pcm_res = core_api->engine->GetPCMFromMusData(musSearched.front());

    auto encoded = pdje_low_level_util::waveform::EncodeWaveformWebps(
        pcm_res, 2, height, pcm_per_pixel, width, -1, 0);
    if (!encoded.ok()) {
        PrintStatusError("PDJE_MIR.SoundToWaveform", encoded.status());
        return out;
    }

    const Array images = encoded.value();

    if (can_use_cache) {
        const PackedByteArray blob = UtilityFunctions::var_to_bytes(images);
        const std::lock_guard<std::mutex> lock(
            s_mir_waveform_cache_write_mutex);
        (void)cache_db->PutBytes(cache_key, blob);
    }

    return SliceImages(images, start_index, end_index);
}
