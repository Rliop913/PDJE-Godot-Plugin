#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "util/function/image/waveform/highlevel/adapter/HighLevelWaveformAdapter.hpp"

#include "util/PDJE_LowLevelUtilAPI.hpp"

#include <mutex>

namespace {

using godot::Array;
using godot::Dictionary;
using godot::Object;
using godot::PackedByteArray;
using godot::PackedFloat32Array;
using godot::PDJE_LowLevelUtilAPI;
using godot::String;
using godot::UtilityFunctions;
using godot::Variant;

std::mutex s_high_level_waveform_cache_write_mutex;

bool
EnvelopeOk(const Dictionary &result)
{
    return result.has("ok") && result["ok"].get_type() == Variant::BOOL &&
           static_cast<bool>(result["ok"]);
}

String
EnvelopeCode(const Dictionary &result)
{
    if (result.has("code") && result["code"].get_type() == Variant::STRING) {
        return static_cast<String>(result["code"]);
    }
    return String("UNKNOWN");
}

String
EnvelopeMessage(const Dictionary &result)
{
    if (result.has("message") &&
        result["message"].get_type() == Variant::STRING) {
        return static_cast<String>(result["message"]);
    }
    return String();
}

Dictionary
EnvelopeData(const Dictionary &result)
{
    if (result.has("data") &&
        result["data"].get_type() == Variant::DICTIONARY) {
        return static_cast<Dictionary>(result["data"]);
    }
    return Dictionary();
}

void
PrintHighLevelError(const String &message)
{
    UtilityFunctions::push_error(message);
}

bool
ReadMusicPath(const Dictionary &music_item, String &music_path)
{
    if (!music_item.has("musicPath") ||
        music_item["musicPath"].get_type() != Variant::STRING) {
        return false;
    }

    music_path = static_cast<String>(music_item["musicPath"]);
    return !music_path.is_empty();
}

String
BuildCacheKey(const String &music_path,
              int           width,
              int           height,
              int           pcm_per_pixel)
{
    return String("MUSR|") + music_path + String("|w:") +
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

Array
PackedFloat32ArrayToArray(const PackedFloat32Array &pcm)
{
    Array out;
    out.resize(pcm.size());
    for (int64_t i = 0; i < pcm.size(); ++i) {
        out[i] = pcm[i];
    }
    return out;
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

bool
ReadWaveformImages(const Dictionary &waveform_result, Array &images)
{
    if (!EnvelopeOk(waveform_result)) {
        return false;
    }

    const Dictionary data = EnvelopeData(waveform_result);
    if (!data.has("images") || data["images"].get_type() != Variant::ARRAY) {
        return false;
    }

    images = static_cast<Array>(data["images"]);
    return true;
}

bool
OpenCache(PDJE_LowLevelUtilAPI &low_level, const String &path)
{
    Dictionary config;
    config["path"]              = path;
    config["create_if_missing"] = true;

    const Dictionary open_result = low_level.KeyValueOpen(config);
    if (EnvelopeOk(open_result)) {
        return true;
    }

    PrintHighLevelError(String("SoundToWaveform failed to open cache DB. ") +
                        EnvelopeCode(open_result) + String(" ") +
                        EnvelopeMessage(open_result));
    return false;
}

} // namespace

namespace godot::pdje_high_level_util::waveform {

Array
SoundToWaveform(Object           *core_api,
                const String     &keyvalue_db_path,
                const Dictionary &music_item,
                int               pcm_per_pixel,
                int               width,
                int               height,
                int               start_index,
                int               end_index)
{
    Array out;

    if (core_api == nullptr) {
        PrintHighLevelError("SoundToWaveform failed. core_api is null.");
        return out;
    }

    if (!core_api->has_method("GetPCMFromMusicData")) {
        PrintHighLevelError("SoundToWaveform failed. core_api must expose "
                            "GetPCMFromMusicData.");
        return out;
    }

    if (keyvalue_db_path.is_empty()) {
        PrintHighLevelError(
            "SoundToWaveform failed. keyvalue_db_path must not be empty.");
        return out;
    }
    if (pcm_per_pixel <= 0 || width <= 0 || height <= 0) {
        PrintHighLevelError("SoundToWaveform failed. width, height, and "
                            "pcm_per_pixel must be positive.");
        return out;
    }

    String music_path;
    if (!ReadMusicPath(music_item, music_path)) {
        PrintHighLevelError("SoundToWaveform failed. music_item must include a "
                            "non-empty musicPath.");
        return out;
    }

    PDJE_LowLevelUtilAPI low_level;
    if (!OpenCache(low_level, keyvalue_db_path)) {
        return out;
    }

    const String cache_key =
        BuildCacheKey(music_path, width, height, pcm_per_pixel);
    const Dictionary cache_result = low_level.KeyValueGetBytes(cache_key);
    if (EnvelopeOk(cache_result)) {
        Array            cached_images;
        const Dictionary cache_data = EnvelopeData(cache_result);
        if (cache_data.has("bytes") &&
            cache_data["bytes"].get_type() == Variant::PACKED_BYTE_ARRAY &&
            DecodeImageArray(static_cast<PackedByteArray>(cache_data["bytes"]),
                             cached_images)) {
            return SliceImages(cached_images, start_index, end_index);
        }
    } else if (EnvelopeCode(cache_result) != "NOT_FOUND") {
        PrintHighLevelError(String("SoundToWaveform cache read failed. ") +
                            EnvelopeCode(cache_result) + String(" ") +
                            EnvelopeMessage(cache_result));
        return out;
    }

    const Variant pcm_result_variant =
        core_api->call("GetPCMFromMusicData", music_item);
    if (pcm_result_variant.get_type() != Variant::DICTIONARY) {
        PrintHighLevelError("SoundToWaveform failed. GetPCMFromMusicData did "
                            "not return a Dictionary.");
        return out;
    }

    const Dictionary   pcm_result = static_cast<Dictionary>(pcm_result_variant);
    PackedFloat32Array pcm;
    int                channel_count = 0;
    if (!ReadPcmResult(pcm_result, pcm, channel_count)) {
        PrintHighLevelError("SoundToWaveform failed. PCM extraction returned "
                            "an invalid payload.");
        return out;
    }

    const Dictionary waveform_result =
        low_level.EncodeWaveformWebps(PackedFloat32ArrayToArray(pcm),
                                      channel_count,
                                      height,
                                      pcm_per_pixel,
                                      width,
                                      -1,
                                      0);
    Array images;
    if (!ReadWaveformImages(waveform_result, images)) {
        PrintHighLevelError(String("SoundToWaveform encode failed. ") +
                            EnvelopeCode(waveform_result) + String(" ") +
                            EnvelopeMessage(waveform_result));
        return out;
    }

    const PackedByteArray blob = UtilityFunctions::var_to_bytes(images);
    {
        const std::lock_guard<std::mutex> lock(
            s_high_level_waveform_cache_write_mutex);
        const Dictionary put_result =
            low_level.KeyValuePutBytes(cache_key, blob);
        if (!EnvelopeOk(put_result)) {
            PrintHighLevelError(String("SoundToWaveform cache write failed. ") +
                                EnvelopeCode(put_result) + String(" ") +
                                EnvelopeMessage(put_result));
        }
    }

    return SliceImages(images, start_index, end_index);
}

} // namespace godot::pdje_high_level_util::waveform
