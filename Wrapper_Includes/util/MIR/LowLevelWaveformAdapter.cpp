#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "util/MIR/LowLevelWaveformAdapter.hpp"

#include "util/function/image/WaveformWebp.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <vector>

namespace {

using godot::Array;
using godot::PackedByteArray;
using godot::PackedFloat32Array;
using godot::Variant;

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

std::vector<float>
ToFloatVector(const PackedFloat32Array &pcm)
{
    std::vector<float> values(static_cast<std::size_t>(pcm.size()));
    if (!values.empty()) {
        std::memcpy(values.data(),
                    pcm.ptr(),
                    static_cast<std::size_t>(pcm.size()) * sizeof(float));
    }
    return values;
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
            images.append(Variant(ToPackedByteArray(image_bytes)));
        }
    }
    return images;
}

} // namespace

namespace godot::pdje_low_level_util::waveform {

PDJE_UTIL::common::Result<Array>
EncodeWaveformWebps(const std::vector<float> &pcm,
                    int                       channel_count,
                    int                       y_pixels,
                    int                       pcm_per_pixel,
                    int                       x_pixels_per_image,
                    int                       compression_level,
                    int                       worker_thread_count)
{
    const auto validation = ValidateWaveformArgs(channel_count,
                                                 y_pixels,
                                                 pcm_per_pixel,
                                                 x_pixels_per_image,
                                                 worker_thread_count);
    if (!validation.ok()) {
        return PDJE_UTIL::common::Result<Array>::failure(validation);
    }

    auto encoded = PDJE_UTIL::function::image::encode_waveform_webps(
        { .pcm                = pcm,
          .channel_count      = static_cast<std::size_t>(channel_count),
          .y_pixels           = static_cast<std::size_t>(y_pixels),
          .pcm_per_pixel      = static_cast<std::size_t>(pcm_per_pixel),
          .x_pixels_per_image = static_cast<std::size_t>(x_pixels_per_image),
          .compression_level  = compression_level,
          .worker_thread_count =
              static_cast<std::size_t>(worker_thread_count) });
    if (!encoded.ok()) {
        return PDJE_UTIL::common::Result<Array>::failure(encoded.status());
    }

    return PDJE_UTIL::common::Result<Array>::success(
        FlattenWaveformBatch(encoded.value()));
}

} // namespace godot::pdje_low_level_util::waveform
