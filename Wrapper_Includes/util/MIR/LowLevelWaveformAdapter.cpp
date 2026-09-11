#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/variant.hpp>

#include "util/MIR/LowLevelWaveformAdapter.hpp"

#include "util/function/image/WaveformWebp.hpp"

#include <cstddef>
#include <cstdint>
#include <cmath>
#include <stdexcept>

namespace {

using godot::Array;
using godot::PackedByteArray;
using godot::Variant;
using godot::pdje_low_level_util::waveform::WaveformEncodeRequest;
using godot::pdje_low_level_util::waveform::WaveformStftConfig;

void
ValidateWaveformArgs(const WaveformEncodeRequest &request)
{
    if (request.channel_count <= 0) {
        throw std::invalid_argument(
            "EncodeWaveformWebps 'channel_count' must be a positive integer.");
    }

    if (request.y_pixels <= 0) {
        throw std::invalid_argument(
            "EncodeWaveformWebps 'y_pixels' must be a positive integer.");
    }

    if (request.pcm_per_pixel <= 0) {
        throw std::invalid_argument(
            "EncodeWaveformWebps 'pcm_per_pixel' must be a positive integer.");
    }

    if (request.x_pixels_per_image <= 0) {
        throw std::invalid_argument(
            "EncodeWaveformWebps 'x_pixels_per_image' must be a positive integer.");
    }

    if (request.worker_thread_count < 0) {
        throw std::invalid_argument(
            "EncodeWaveformWebps 'worker_thread_count' must be zero or greater.");
    }
}

void
ValidateRgbWaveformArgs(const WaveformStftConfig &stft)
{
    if (stft.target_window <
            static_cast<int>(PDJE_PARALLEL::WINDOW_LIST::BLACKMAN) ||
        stft.target_window > static_cast<int>(PDJE_PARALLEL::WINDOW_LIST::NONE)) {
        throw std::invalid_argument(
            "EncodeWaveformWebps 'target_window' must be a valid STFT window enum value.");
    }

    if (stft.window_size_exp < 6 || stft.window_size_exp >= 31) {
        throw std::invalid_argument(
            "EncodeWaveformWebps 'window_size_exp' must be in the range [6, 31).");
    }

    if (!std::isfinite(stft.overlap_ratio) || stft.overlap_ratio < 0.0f ||
        stft.overlap_ratio >= 1.0f) {
        throw std::invalid_argument(
            "EncodeWaveformWebps 'overlap_ratio' must be in the range [0.0, 1.0).");
    }
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

Array
EncodeWaveformWebps(const WaveformEncodeRequest &request)
{
    ValidateWaveformArgs(request);

    const PDJE_UTIL::function::image::EncodeWaveformWebpArgs waveform_args{
        .pcm                = request.pcm,
        .channel_count      = static_cast<std::size_t>(request.channel_count),
        .y_pixels           = static_cast<std::size_t>(request.y_pixels),
        .pcm_per_pixel      = static_cast<std::size_t>(request.pcm_per_pixel),
        .x_pixels_per_image =
            static_cast<std::size_t>(request.x_pixels_per_image),
        .compression_level  = request.compression_level,
        .worker_thread_count =
            static_cast<std::size_t>(request.worker_thread_count),
    };

    if (request.stft.has_value()) {
        ValidateRgbWaveformArgs(*request.stft);

        const auto encoded = PDJE_UTIL::function::image::encode_waveform_webps(
            waveform_args,
            { .target_window = static_cast<PDJE_PARALLEL::WINDOW_LIST>(
                  request.stft->target_window),
              .window_size_exp = request.stft->window_size_exp,
              .overlap_ratio   = request.stft->overlap_ratio,
              .post_process    = {} });
        return FlattenWaveformBatch(encoded);
    }

    return FlattenWaveformBatch(
        PDJE_UTIL::function::image::encode_waveform_webps(waveform_args));
}

} // namespace godot::pdje_low_level_util::waveform
