#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "util/function/image/waveform/adapter/LowLevelWaveformAdapter.hpp"

#include "util/common/bridge/LowLevelUtilCommon.hpp"
#include "util/function/image/WaveformWebp.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace {

using godot::Array;
using godot::Dictionary;
using godot::PackedByteArray;
using godot::String;
using godot::pdje_low_level_util::common::MakeStatusResult;
using godot::pdje_low_level_util::common::MakeSuccessResult;
using godot::pdje_low_level_util::common::VariantToFloat;

struct WaveformPcmParseResult {
    std::vector<float>        values;
    PDJE_UTIL::common::Status status;
};

Dictionary
MakeWaveformPayload()
{
    Dictionary payload;
    payload["images"]              = Array();
    payload["order"]               = String("LLRR");
    payload["layout"]              = String("channel_major");
    payload["channel_count"]       = static_cast<int64_t>(0);
    payload["images_per_channel"]  = static_cast<int64_t>(0);
    payload["total_images"]        = static_cast<int64_t>(0);
    payload["y_pixels"]            = static_cast<int64_t>(0);
    payload["pcm_per_pixel"]       = static_cast<int64_t>(0);
    payload["x_pixels_per_image"]  = static_cast<int64_t>(0);
    payload["compression_level"]   = static_cast<int64_t>(-1);
    payload["worker_thread_count"] = static_cast<int64_t>(0);
    payload["pcm_length"]          = static_cast<int64_t>(0);
    return payload;
}

Dictionary
MakeWaveformData(std::size_t  pcm_length,
                 int          channel_count,
                 int          y_pixels,
                 int          pcm_per_pixel,
                 int          x_pixels_per_image,
                 int          compression_level,
                 int          worker_thread_count,
                 const Array &images,
                 int64_t      images_per_channel,
                 int64_t      total_images)
{
    Dictionary data             = MakeWaveformPayload();
    data["images"]              = images;
    data["channel_count"]       = static_cast<int64_t>(channel_count);
    data["images_per_channel"]  = images_per_channel;
    data["total_images"]        = total_images;
    data["y_pixels"]            = static_cast<int64_t>(y_pixels);
    data["pcm_per_pixel"]       = static_cast<int64_t>(pcm_per_pixel);
    data["x_pixels_per_image"]  = static_cast<int64_t>(x_pixels_per_image);
    data["compression_level"]   = static_cast<int64_t>(compression_level);
    data["worker_thread_count"] = static_cast<int64_t>(worker_thread_count);
    data["pcm_length"]          = static_cast<int64_t>(pcm_length);
    return data;
}

PDJE_UTIL::common::Status
ValidateWaveformArgs(int channel_count,
                     int y_pixels,
                     int pcm_per_pixel,
                     int x_pixels_per_image,
                     int worker_thread_count)
{
    if (channel_count <= 0) {
        return {
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "EncodeWaveformWebps 'channel_count' must be a positive integer."
        };
    }

    if (y_pixels <= 0) {
        return { PDJE_UTIL::common::StatusCode::invalid_argument,
                 "EncodeWaveformWebps 'y_pixels' must be a positive integer." };
    }

    if (pcm_per_pixel <= 0) {
        return {
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "EncodeWaveformWebps 'pcm_per_pixel' must be a positive integer."
        };
    }

    if (x_pixels_per_image <= 0) {
        return { PDJE_UTIL::common::StatusCode::invalid_argument,
                 "EncodeWaveformWebps 'x_pixels_per_image' must be a positive "
                 "integer." };
    }

    if (worker_thread_count < 0) {
        return {
            PDJE_UTIL::common::StatusCode::invalid_argument,
            "EncodeWaveformWebps 'worker_thread_count' must be zero or greater."
        };
    }

    return {};
}

WaveformPcmParseResult
ParseWaveformPcm(const Array &pcm)
{
    WaveformPcmParseResult parsed;
    parsed.values.reserve(static_cast<std::size_t>(pcm.size()));

    for (int64_t i = 0; i < pcm.size(); ++i) {
        float sample = 0.0F;
        if (!VariantToFloat(pcm[i], sample)) {
            parsed.status = { PDJE_UTIL::common::StatusCode::invalid_argument,
                              "EncodeWaveformWebps 'pcm' must contain only int "
                              "or float values; invalid value at index " +
                                  std::to_string(i) + "." };
            parsed.values.clear();
            return parsed;
        }

        parsed.values.push_back(sample);
    }

    parsed.status = {};
    return parsed;
}

PackedByteArray
ToPackedByteArray(const PDJE_UTIL::function::image::EncodedWebpBytes &bytes)
{
    PackedByteArray packed;
    packed.resize(static_cast<int64_t>(bytes.size()));
    for (int64_t i = 0; i < static_cast<int64_t>(bytes.size()); ++i) {
        packed.set(i, bytes[static_cast<std::size_t>(i)]);
    }
    return packed;
}

Array
FlattenWaveformBatch(const PDJE_UTIL::function::image::WaveformWebpBatch &batch)
{
    Array images;
    for (const auto &channel_batch : batch) {
        for (const auto &image_bytes : channel_batch) {
            images.append(ToPackedByteArray(image_bytes));
        }
    }
    return images;
}

} // namespace

namespace godot::pdje_low_level_util::waveform {

Dictionary
EncodeWaveformWebps(const Array &pcm,
                    int          channel_count,
                    int          y_pixels,
                    int          pcm_per_pixel,
                    int          x_pixels_per_image,
                    int          compression_level,
                    int          worker_thread_count)
{
    const Dictionary base_data =
        MakeWaveformData(static_cast<std::size_t>(pcm.size()),
                         channel_count,
                         y_pixels,
                         pcm_per_pixel,
                         x_pixels_per_image,
                         compression_level,
                         worker_thread_count,
                         Array(),
                         0,
                         0);

    const auto parsed_pcm = ParseWaveformPcm(pcm);
    if (!parsed_pcm.status.ok()) {
        return MakeStatusResult(false, parsed_pcm.status, base_data);
    }

    const auto validation = ValidateWaveformArgs(channel_count,
                                                 y_pixels,
                                                 pcm_per_pixel,
                                                 x_pixels_per_image,
                                                 worker_thread_count);
    if (!validation.ok()) {
        return MakeStatusResult(false, validation, base_data);
    }

    auto encoded = PDJE_UTIL::function::image::encode_waveform_webps(
        { .pcm                = std::span<const float>(parsed_pcm.values.data(),
                                        parsed_pcm.values.size()),
          .channel_count      = static_cast<std::size_t>(channel_count),
          .y_pixels           = static_cast<std::size_t>(y_pixels),
          .pcm_per_pixel      = static_cast<std::size_t>(pcm_per_pixel),
          .x_pixels_per_image = static_cast<std::size_t>(x_pixels_per_image),
          .compression_level  = compression_level,
          .worker_thread_count =
              static_cast<std::size_t>(worker_thread_count) });
    if (!encoded.ok()) {
        return MakeStatusResult(false, encoded.status(), base_data);
    }

    const auto   &batch         = encoded.value();
    const int64_t channel_total = static_cast<int64_t>(batch.size());
    const int64_t images_per_channel =
        batch.empty() ? 0 : static_cast<int64_t>(batch.front().size());
    const Array images = FlattenWaveformBatch(batch);

    return MakeSuccessResult(
        MakeWaveformData(parsed_pcm.values.size(),
                         static_cast<int>(channel_total),
                         y_pixels,
                         pcm_per_pixel,
                         x_pixels_per_image,
                         compression_level,
                         worker_thread_count,
                         images,
                         images_per_channel,
                         images_per_channel * channel_total));
}

} // namespace godot::pdje_low_level_util::waveform
