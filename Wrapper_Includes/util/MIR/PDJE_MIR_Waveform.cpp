#include "WaveformWebp.hpp"
#include "pdje_util_common.hpp"
#include "util/MIR/PDJE_MIR.hpp"

#include "PDJE_Core_Wrapper.hpp"
#include "util/MIR/PDJE_MIR_CacheKeys.hpp"
#include "util/MIR/LowLevelWaveformAdapter.hpp"
#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"
#include "util/db/keyvalue/PDJE_KeyValueDB.hpp"

#include <algorithm>
#include <optional>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <cstdint>
#include <mutex>
#include <span>
#include <utility>

using namespace godot;

namespace {

using godot::pdje_public_util::common::print_method_error;
using godot::pdje_public_util::common::value_or_error;
using godot::pdje_mir_internal::cache_keys::BuildWaveformCacheKey;
using godot::pdje_mir_internal::cache_keys::BuildWaveformRgbCacheKey;
using godot::pdje_low_level_util::waveform::WaveformEncodeRequest;
using godot::pdje_low_level_util::waveform::WaveformStftConfig;

struct WaveformRenderRequest {
    const char                       *method_name = nullptr;
    String                            music_title;
    String                            composer;
    float                             bpm           = 0.0f;
    int                               pcm_per_pixel = 0;
    int                               width         = 0;
    int                               height        = 0;
    int                               start_index   = 0;
    int                               end_index     = -1;
    std::optional<WaveformStftConfig> stft;
};

String
BuildRequestCacheKey(const String                &cache_source_key,
                     const WaveformRenderRequest &request)
{
    if (!request.stft.has_value()) {
        return BuildWaveformCacheKey(cache_source_key,
                                     request.width,
                                     request.height,
                                     request.pcm_per_pixel);
    }

    return BuildWaveformRgbCacheKey(cache_source_key,
                                    request.width,
                                    request.height,
                                    request.pcm_per_pixel,
                                    request.stft->target_window,
                                    request.stft->window_size_exp,
                                    request.stft->overlap_ratio);
}

bool
ValidateWaveformRenderRequest(const WaveformRenderRequest &request)
{
    if (request.pcm_per_pixel <= 0 || request.width <= 0 || request.height <= 0) {
        print_method_error(request.method_name,
                           "width, height, and pcm_per_pixel must be positive");
        return false;
    }

    return true;
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

Array
EncodeWaveformImages(const std::vector<float>    &pcm,
                     const WaveformRenderRequest &request)
{
    return pdje_low_level_util::waveform::EncodeWaveformWebps(
        { .pcm                = std::span<const float>(pcm.data(), pcm.size()),
          .channel_count      = 2,
          .y_pixels           = request.height,
          .pcm_per_pixel      = request.pcm_per_pixel,
          .x_pixels_per_image = request.width,
          .compression_level  = -1,
          .worker_thread_count = 0,
          .stft               = request.stft });
}

Array
ExecuteWaveformRequest(PDJE_Wrapper                *core_api,
                       PDJE_KeyValueDB             *cache_db,
                       const WaveformRenderRequest &request,
                       std::mutex                  &cache_write_mutex)
{
    Array out;

    if (core_api == nullptr) {
        print_method_error(request.method_name, "core_api is null");
        return out;
    }
    if (!core_api->engine.has_value()) {
        print_method_error(request.method_name, "core_api is null");
        return out;
    }
    if (!ValidateWaveformRenderRequest(request)) {
        return out;
    }

    auto mus_searched = core_api->engine->SearchMusic(GStrToCStr(request.music_title),
                                                      GStrToCStr(request.composer),
                                                      request.bpm);
    if (mus_searched.empty()) {
        return {};
    }

    String cache_source_key;
    pdje_public_util::common::BuildCacheSourceKey(request.music_title,
                                                  request.composer,
                                                  mus_searched.front().bpm,
                                                  cache_source_key);

    const bool can_use_cache = pdje_public_util::common::CheckDB(cache_db);
    const String cache_key = BuildRequestCacheKey(cache_source_key, request);
    if (can_use_cache && cache_db->Contains(cache_key)) {
        const PackedByteArray cached_blob = cache_db->GetBytes(cache_key);
        Array                 cached_images;
        if (DecodeImageArray(cached_blob, cached_images)) {
            return SliceImages(cached_images,
                               request.start_index,
                               request.end_index);
        }
    }

    const auto pcm = core_api->engine->GetPCMFromMusData(mus_searched.front(), 2);
    auto encoded = value_or_error(request.method_name,
                                  [&]() {
                                      return EncodeWaveformImages(pcm, request);
                                  });
    if (!encoded.has_value()) {
        return out;
    }

    const Array images = *encoded;
    if (can_use_cache) {
        const PackedByteArray blob = UtilityFunctions::var_to_bytes(images);
        const std::lock_guard<std::mutex> lock(cache_write_mutex);
        (void)cache_db->PutBytes(cache_key, blob);
    }

    return SliceImages(images, request.start_index, request.end_index);
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
    return ExecuteWaveformRequest(
        core_api,
        cache_db,
        { .method_name   = "PDJE_MIR.SoundToWaveform",
          .music_title   = musicTitle,
          .composer      = composer,
          .bpm           = bpm,
          .pcm_per_pixel = pcm_per_pixel,
          .width         = width,
          .height        = height,
          .start_index   = start_index,
          .end_index     = end_index },
        s_mir_waveform_cache_write_mutex);
}

Array
PDJE_MIR::SoundToRGBWaveform(PDJE_Wrapper    *core_api,
                             PDJE_KeyValueDB *cache_db,
                             String           musicTitle,
                             String           composer,
                             float            bpm,
                             int              pcm_per_pixel,
                             int              width,
                             int              height,
                             int              start_index,
                             int              end_index,
                             int              target_window,
                             int              window_size_exp,
                             float            overlap_ratio)
{
    return ExecuteWaveformRequest(
        core_api,
        cache_db,
        { .method_name   = "PDJE_MIR.SoundToRGBWaveform",
          .music_title   = musicTitle,
          .composer      = composer,
          .bpm           = bpm,
          .pcm_per_pixel = pcm_per_pixel,
          .width         = width,
          .height        = height,
          .start_index   = start_index,
          .end_index     = end_index,
          .stft          = WaveformStftConfig{ .target_window = target_window,
                                               .window_size_exp =
                                                   window_size_exp,
                                               .overlap_ratio = overlap_ratio } },
        s_mir_waveform_cache_write_mutex);
}
