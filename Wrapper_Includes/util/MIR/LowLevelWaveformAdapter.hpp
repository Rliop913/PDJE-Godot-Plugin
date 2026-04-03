#pragma once

#include "util/common/Result.hpp"
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <vector>

namespace godot {

namespace pdje_low_level_util::waveform {

PDJE_UTIL::common::Result<Array>
EncodeWaveformWebps(const std::vector<float> &pcm,
                    int                       channel_count,
                    int                       y_pixels,
                    int                       pcm_per_pixel,
                    int                       x_pixels_per_image,
                    int                       compression_level,
                    int                       worker_thread_count);

}

} // namespace godot
