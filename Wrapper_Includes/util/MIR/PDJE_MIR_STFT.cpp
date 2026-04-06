#include "util/MIR/PDJE_MIR.hpp"

#include "util/MIR/PDJE_MIR_CacheKeys.hpp"
#include "util/MIR/PDJE_StftResult.hpp"
#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"
#include "util/function/stft/STFT_Parallel.hpp"
#include "variant/color.hpp"
#include "variant/packed_byte_array.hpp"
#include "variant/packed_color_array.hpp"
#include "variant/typed_array.hpp"
#include "variant/utility_functions.hpp"
#include "variant/variant.hpp"

#include <algorithm>
#include <cmath>
#include <godot_cpp/variant/packed_float32_array.hpp>

#include <cstring>
#include <memory>
#include <mutex>
#include <span>
#include <string>
#include <vector>

using namespace godot;

namespace {

using godot::pdje_mir_internal::cache_keys::BuildStftMusicCacheKey;
using godot::pdje_public_util::common::print_method_error;

PackedFloat32Array
ToPackedFloat32Array(const std::vector<float> &values)
{
    PackedFloat32Array packed;
    if (values.empty()) {
        return packed;
    }
    packed.resize(static_cast<int64_t>(values.size()));

    std::memcpy(packed.ptrw(), values.data(), values.size() * sizeof(float));
    return packed;
}

std::vector<std::vector<float>>
SplitChannel(const PackedFloat32Array &arr, const int ch)
{
    if (ch <= 0 || arr.is_empty() || (arr.size() % ch) != 0) {
        return {};
    }

    std::vector<std::vector<float>> output(ch);
    const auto                      frameSZ   = arr.size() / ch;
    int                             ch_offset = 0;
    for (auto &v : output) {
        v.resize(frameSZ);
        for (int i = 0; i < frameSZ; ++i) {
            v[i] = arr[(i * ch) + ch_offset];
        }
        ++ch_offset;
    }

    return output;
}

std::vector<std::vector<float>>
SplitChannel(const std::vector<float> &arr, const int ch)
{
    if (ch <= 0 || arr.empty() || (arr.size() % ch) != 0) {
        return {};
    }

    std::vector<std::vector<float>> output(ch);
    const auto                      frameSZ   = arr.size() / ch;
    int                             ch_offset = 0;
    for (auto &v : output) {
        v.resize(frameSZ);
        for (int i = 0; i < frameSZ; ++i) {
            v[i] = arr[(i * ch) + ch_offset];
        }
        ++ch_offset;
    }

    return output;
}

} // namespace

PDJE_MIR::PDJE_MIR() = default;

PackedColorArray
PDJE_MIR::STFT_MUSIC(PDJE_Wrapper    *core_api,
                     PDJE_KeyValueDB *cache_db,
                     String           musicTitle,
                     String           composer,
                     float            bpm,
                     int              target_window,
                     int              window_size_exp,
                     float            overlap_ratio)
{
    if (core_api == nullptr) {
        print_method_error("PDJE_MIR.SoundToWaveform", "core_api is null");
        return {};
    }
    if (!core_api->engine.has_value()) {
        print_method_error("PDJE_MIR.SoundToWaveform", "core_api is null");
        return {};
    }
    auto musSearched = core_api->engine->SearchMusic(
        GStrToCStr(musicTitle), GStrToCStr(composer), bpm);
    if (musSearched.empty()) {
        print_method_error("PDJE_MIR.STFT_MUSIC", "music data dot found");
        return {};
    }
    bool can_use_cache = pdje_public_util::common::CheckDB(cache_db);
    if (window_size_exp < 6 || window_size_exp >= 31) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA",
                           "'window_size_exp' must be in the range [6, 31)");
        return {};
    }
    overlap_ratio = std::clamp(overlap_ratio, 0.0f, 1.0f);
    if (overlap_ratio == 1.0f) {
        overlap_ratio -= 0.1f;
    }
    overlap_ratio = std::trunc(overlap_ratio * 100.0f) / 100.0f;
    if (!stft_ptr) {
        stft_ptr = std::make_unique<PDJE_PARALLEL::STFT>();
    }

    String cache_key;
    pdje_public_util::common::BuildCacheSourceKey(
        musicTitle, composer, musSearched.front().bpm, cache_key);

    cache_key = BuildStftMusicCacheKey(cache_key,
                                       target_window,
                                       window_size_exp,
                                       overlap_ratio);

    if (can_use_cache) {
        PackedByteArray cached_blob;
        bool            cached_found = false;
        if (cache_db->TryGetBytesSilently(
                cache_key, cached_blob, cached_found) &&
            cached_found) {
            const Variant decoded = UtilityFunctions::bytes_to_var(cached_blob);
            if (decoded.get_type() == Variant::PACKED_COLOR_ARRAY) {
                return decoded;
            }
        }
    }

    auto pcm = core_api->engine->GetPCMFromMusData(musSearched.front(), 1);

    PackedColorArray output;

    PDJE_PARALLEL::POST_PROCESS post_process{ .to_bin            = true,
                                              .toPower           = true,
                                              .mel_scale         = true,
                                              .to_db             = true,
                                              .normalize_min_max = true,
                                              .to_rgb            = true };
    auto [real, imag] = stft_ptr.value()->calculate(
        pcm,
        static_cast<PDJE_PARALLEL::WINDOW_LIST>(target_window),
        window_size_exp,
        overlap_ratio,
        post_process);

    if (real.empty()) {
        return {};
    }

    const auto itrSZ = real.size() / 3; // RGB

    for (unsigned int i = 0; i < itrSZ; ++i) {
        const auto base = i * 3;
        output.push_back(Color(real[base], real[base + 1], real[base + 2]));
    }
    if (can_use_cache) {
        const PackedByteArray blob = UtilityFunctions::var_to_bytes(output);
        const std::lock_guard<std::mutex> lock(mir_stft_cache_write_mutex);
        cache_db->PutBytes(cache_key, blob);
    }

    return output;
}

TypedArray<Ref<PDJE_StftResult>>
PDJE_MIR::STFT_PCM_DATA(PackedFloat32Array pcm,
                        int                channel_count,
                        int                target_window,
                        int                window_size_exp,
                        float              overlap_ratio,
                        bool               toPower,
                        bool               to_bin,
                        bool               normalize_min_max,
                        bool               mel_scale,
                        bool               to_db,
                        bool               to_rgb)
{
    if (pcm.is_empty()) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA", "'pcm' must not be empty");
        return {};
    }
    if (channel_count < 1) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA", "invalid channel_count");
        return {};
    }

    if (window_size_exp < 6 || window_size_exp >= 31) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA",
                           "'window_size_exp' must be in the range [6, 31)");
        return {};
    }
    overlap_ratio = std::clamp(overlap_ratio, 0.0f, 1.0f);
    if (overlap_ratio == 1.0f) {
        overlap_ratio -= 0.1f;
    }
    if (!stft_ptr) {
        stft_ptr = std::make_unique<PDJE_PARALLEL::STFT>();
    }

    auto                             chs = SplitChannel(pcm, channel_count);
    TypedArray<Ref<PDJE_StftResult>> output;
    for (auto &samples : chs) {

        PDJE_PARALLEL::POST_PROCESS post_process{ .to_bin    = to_bin,
                                                  .toPower   = toPower,
                                                  .mel_scale = mel_scale,
                                                  .to_db     = to_db,
                                                  .normalize_min_max =
                                                      normalize_min_max,
                                                  .to_rgb = to_rgb };
        auto [real, imag] = stft_ptr.value()->calculate(
            samples,
            static_cast<PDJE_PARALLEL::WINDOW_LIST>(target_window),
            window_size_exp,
            overlap_ratio,
            post_process);
        Ref<PDJE_StftResult> result;
        result.instantiate();
        result->set_real(ToPackedFloat32Array(real));
        result->set_imag(ToPackedFloat32Array(imag));
        output.push_back(result);
    }
    return output;
}

PDJE_MIR::~PDJE_MIR()
{
    stft_ptr.reset();
}
