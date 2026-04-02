#include "util/PDJE_MIR.hpp"

#include "PDJE_Core_Wrapper.hpp"
#include "util/PDJE_StftResult.hpp"
#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/db/keyvalue/PDJE_KeyValueDB.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"
#include "util/function/image/waveform/adapter/LowLevelWaveformAdapter.hpp"
#include "util/function/stft/STFT_Parallel.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

using namespace godot;

VARIANT_ENUM_CAST(PDJE_MIR::STFT_WINDOW_LIST);

namespace godot::pdje_domain_internal {

struct StftState {
    std::unique_ptr<PDJE_PARALLEL::STFT> stft;
};

} // namespace godot::pdje_domain_internal

namespace {

using godot::pdje_low_level_util::common::StatusCodeToGodotCode;
using godot::pdje_low_level_util::common::StatusMessageToGodot;
using godot::pdje_public_util::common::print_method_error;

std::mutex s_mir_waveform_cache_write_mutex;

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
PrintStatusError(const char *method_name, const PDJE_UTIL::common::Status &status)
{
    print_method_error(method_name, FormatStatusDetail(status));
}

bool
TryReadStringField(const Dictionary &music_item,
                   const char       *key,
                   String           &out_value)
{
    out_value = String();
    if (!music_item.has(key) || music_item[key].get_type() != Variant::STRING) {
        return false;
    }

    out_value = static_cast<String>(music_item[key]);
    return !out_value.is_empty();
}

void
ReadBpmField(const Dictionary &music_item, double &out_bpm, bool &out_has_bpm)
{
    out_bpm     = -1.0;
    out_has_bpm = false;
    if (!music_item.has("bpm")) {
        return;
    }

    const Variant bpm_value = music_item["bpm"];
    switch (bpm_value.get_type()) {
    case Variant::INT:
        out_bpm     = static_cast<double>(static_cast<int64_t>(bpm_value));
        out_has_bpm = true;
        return;
    case Variant::FLOAT:
        out_bpm     = static_cast<double>(bpm_value);
        out_has_bpm = std::isfinite(out_bpm);
        if (!out_has_bpm) {
            out_bpm = -1.0;
        }
        return;
    default:
        return;
    }
}

String
NormalizeBpmKey(double bpm, bool has_bpm)
{
    if (!has_bpm) {
        return String();
    }

    if (std::floor(bpm) == bpm) {
        return String::num_int64(static_cast<int64_t>(bpm));
    }
    return String::num_real(bpm);
}

bool
BuildCacheSourceKey(const Dictionary &music_item, String &out_source_key)
{
    String music_path;
    String title;
    String composer;
    double bpm     = -1.0;
    bool   has_bpm = false;

    TryReadStringField(music_item, "musicPath", music_path);
    TryReadStringField(music_item, "title", title);
    TryReadStringField(music_item, "composer", composer);
    ReadBpmField(music_item, bpm, has_bpm);

    if (!music_path.is_empty()) {
        out_source_key = String("path:") + music_path;
        return true;
    }

    if (title.is_empty() && composer.is_empty() && !has_bpm) {
        out_source_key = String();
        return false;
    }

    out_source_key = String("query|title:") + title + String("|composer:") +
                     composer + String("|bpm:") + NormalizeBpmKey(bpm, has_bpm);
    return true;
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

    int64_t start = start_index < 0 ? 0 : static_cast<int64_t>(start_index);
    int64_t end   = end_index < 0 ? static_cast<int64_t>(images.size()) - 1
                                  : static_cast<int64_t>(end_index);
    const int64_t max_index = static_cast<int64_t>(images.size()) - 1;

    if (start > max_index) {
        start = max_index;
    }
    if (end > max_index) {
        end = max_index;
    }
    if (start > end) {
        const int64_t temp = start;
        start              = end;
        end                = temp;
    }

    for (int64_t i = start; i <= end; ++i) {
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

bool
ReadPcmResult(const Dictionary   &pcm_result,
              PackedFloat32Array &pcm,
              int                &channel_count)
{
    if (!pcm_result.has("pcm") ||
        pcm_result["pcm"].get_type() != Variant::PACKED_FLOAT32_ARRAY) {
        return false;
    }
    if (!pcm_result.has("channel_count") ||
        pcm_result["channel_count"].get_type() != Variant::INT) {
        return false;
    }

    pcm = static_cast<PackedFloat32Array>(pcm_result["pcm"]);
    channel_count =
        static_cast<int>(static_cast<int64_t>(pcm_result["channel_count"]));
    return !pcm.is_empty() && channel_count > 0;
}

PackedFloat32Array
ToPackedFloat32Array(const std::vector<float> &values)
{
    PackedFloat32Array packed;
    packed.resize(static_cast<int64_t>(values.size()));
    if (values.empty()) {
        return packed;
    }

    std::memcpy(
        packed.ptrw(), values.data(), values.size() * sizeof(float));
    return packed;
}

String
StftIndexString(int64_t index)
{
    return String(std::to_string(index).c_str());
}

} // namespace

void
PDJE_MIR::_bind_methods()
{
    BIND_ENUM_CONSTANT(BLACKMAN);
    BIND_ENUM_CONSTANT(BLACKMAN_HARRIS);
    BIND_ENUM_CONSTANT(BLACKMAN_NUTTALL);
    BIND_ENUM_CONSTANT(HANNING);
    BIND_ENUM_CONSTANT(NUTTALL);
    BIND_ENUM_CONSTANT(FLATTOP);
    BIND_ENUM_CONSTANT(GAUSSIAN);
    BIND_ENUM_CONSTANT(HAMMING);
    BIND_ENUM_CONSTANT(NONE);

    ClassDB::bind_method(D_METHOD("SoundToWaveform",
                                  "core_api",
                                  "cache_db",
                                  "music_item",
                                  "pcm_per_pixel",
                                  "width",
                                  "height",
                                  "start_index",
                                  "end_index"),
                         &PDJE_MIR::SoundToWaveform,
                         DEFVAL(4096),
                         DEFVAL(256),
                         DEFVAL(0),
                         DEFVAL(-1));
    ClassDB::bind_method(D_METHOD("STFTCreate"), &PDJE_MIR::STFTCreate);
    ClassDB::bind_method(D_METHOD("STFTDestroy"), &PDJE_MIR::STFTDestroy);
    ClassDB::bind_method(D_METHOD("STFT_PCM_DATA",
                                  "pcm",
                                  "channel_count",
                                  "target_window",
                                  "window_size_exp",
                                  "overlap_ratio",
                                  "to_power"),
                         &PDJE_MIR::STFT_PCM_DATA,
                         DEFVAL(HANNING),
                         DEFVAL(10),
                         DEFVAL(0.5f),
                         DEFVAL(false));
}

Array
PDJE_MIR::SoundToWaveform(PDJE_Wrapper   *core_api,
                          PDJE_KeyValueDB *cache_db,
                          Dictionary      music_item,
                          int             pcm_per_pixel,
                          int             width,
                          int             height,
                          int             start_index,
                          int             end_index)
{
    Array out;

    if (core_api == nullptr) {
        print_method_error("PDJE_MIR.SoundToWaveform", "core_api is null");
        return out;
    }
    if (pcm_per_pixel <= 0 || width <= 0 || height <= 0) {
        print_method_error("PDJE_MIR.SoundToWaveform",
                           "width, height, and pcm_per_pixel must be positive");
        return out;
    }

    String cache_source_key;
    if (!BuildCacheSourceKey(music_item, cache_source_key)) {
        print_method_error("PDJE_MIR.SoundToWaveform",
                           "music_item must include musicPath or search clues");
        return out;
    }

    bool can_use_cache = true;
    if (cache_db == nullptr) {
        print_method_error("PDJE_MIR.SoundToWaveform",
                           "cache_db is null; continuing without cache");
        can_use_cache = false;
    } else if (!cache_db->IsOpen()) {
        print_method_error("PDJE_MIR.SoundToWaveform",
                           "cache_db is not open; continuing without cache");
        can_use_cache = false;
    }

    const String cache_key =
        BuildCacheKey(cache_source_key, width, height, pcm_per_pixel);
    if (can_use_cache) {
        PackedByteArray cached_blob;
        bool            cached_found = false;
        if (cache_db->TryGetBytesSilently(cache_key, cached_blob, cached_found) &&
            cached_found) {
            Array cached_images;
            if (DecodeImageArray(cached_blob, cached_images)) {
                return SliceImages(cached_images, start_index, end_index);
            }
        }
    }

    const Dictionary   pcm_result = core_api->GetPCMFromMusicData(music_item);
    PackedFloat32Array pcm;
    int                channel_count = 0;
    if (!ReadPcmResult(pcm_result, pcm, channel_count)) {
        print_method_error("PDJE_MIR.SoundToWaveform",
                           "PCM extraction returned an invalid payload");
        return out;
    }

    auto encoded = pdje_low_level_util::waveform::EncodeWaveformWebps(pcm,
                                                                      channel_count,
                                                                      height,
                                                                      pcm_per_pixel,
                                                                      width,
                                                                      -1,
                                                                      0);
    if (!encoded.ok()) {
        PrintStatusError("PDJE_MIR.SoundToWaveform", encoded.status());
        return out;
    }

    const Array images = encoded.value();

    if (can_use_cache) {
        const PackedByteArray blob = UtilityFunctions::var_to_bytes(images);
        const std::lock_guard<std::mutex> lock(s_mir_waveform_cache_write_mutex);
        (void)cache_db->PutBytes(cache_key, blob);
    }

    return SliceImages(images, start_index, end_index);
}

bool
PDJE_MIR::STFTCreate()
{
    if (!stft_state_) {
        stft_state_ = std::make_unique<pdje_domain_internal::StftState>();
    }

    if (!stft_state_->stft) {
        stft_state_->stft = std::make_unique<PDJE_PARALLEL::STFT>();
    }

    return true;
}

bool
PDJE_MIR::STFTDestroy()
{
    stft_state_.reset();
    return true;
}

Ref<PDJE_StftResult>
PDJE_MIR::STFT_PCM_DATA(Array pcm,
                        int   channel_count,
                        int   target_window,
                        int   window_size_exp,
                        float overlap_ratio,
                        bool  to_power)
{
    if (pcm.is_empty()) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA", "'pcm' must not be empty");
        return Ref<PDJE_StftResult>();
    }

    if (channel_count != 1) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA",
                           "'channel_count' must be 1 for mono PCM");
        return Ref<PDJE_StftResult>();
    }

    if (target_window < static_cast<int>(PDJE_PARALLEL::WINDOW_LIST::BLACKMAN) ||
        target_window > static_cast<int>(PDJE_PARALLEL::WINDOW_LIST::NONE)) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA",
                           "'target_window' must be between 0 and 8");
        return Ref<PDJE_StftResult>();
    }

    if (window_size_exp < 6 || window_size_exp >= 31) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA",
                           "'window_size_exp' must be in the range [6, 31)");
        return Ref<PDJE_StftResult>();
    }

    if (!(overlap_ratio >= 0.0f && overlap_ratio < 1.0f)) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA",
                           "'overlap_ratio' must be in the range [0.0, 1.0)");
        return Ref<PDJE_StftResult>();
    }

    std::vector<float> samples;
    samples.reserve(static_cast<std::size_t>(pcm.size()));
    for (int64_t i = 0; i < pcm.size(); ++i) {
        float sample = 0.0f;
        if (!pdje_low_level_util::common::VariantToFloat(pcm[i], sample)) {
            print_method_error("PDJE_MIR.STFT_PCM_DATA",
                               "'pcm' must contain only numeric values; "
                               "invalid index " +
                                   StftIndexString(i));
            return Ref<PDJE_StftResult>();
        }
        samples.push_back(sample);
    }

    PDJE_PARALLEL::STFT  temporary_stft;
    PDJE_PARALLEL::STFT *stft =
        stft_state_ && stft_state_->stft ? stft_state_->stft.get()
                                         : &temporary_stft;
    auto [first_output, second_output] =
        stft->calculate(samples,
                        static_cast<PDJE_PARALLEL::WINDOW_LIST>(target_window),
                        window_size_exp,
                        overlap_ratio,
                        to_power);

    if (first_output.empty() && second_output.empty()) {
        print_method_error("PDJE_MIR.STFT_PCM_DATA",
                           "backend returned an empty result");
        return Ref<PDJE_StftResult>();
    }

    Ref<PDJE_StftResult> result;
    result.instantiate();
    if (to_power) {
        result->set_power(ToPackedFloat32Array(first_output));
        return result;
    }

    result->set_real(ToPackedFloat32Array(first_output));
    result->set_imag(ToPackedFloat32Array(second_output));
    return result;
}

PDJE_MIR::PDJE_MIR() = default;

PDJE_MIR::~PDJE_MIR()
{
    stft_state_.reset();
}

void
PDJE_MIR::_ready()
{
}
