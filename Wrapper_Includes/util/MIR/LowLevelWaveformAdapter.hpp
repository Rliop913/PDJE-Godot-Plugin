#pragma once

#include "STFT_Parallel.hpp"
#include <godot_cpp/variant/array.hpp>
#include <optional>
#include <span>

namespace godot {

namespace pdje_low_level_util::waveform {

struct WaveformStftConfig {
    int   target_window =
        static_cast<int>(PDJE_PARALLEL::WINDOW_LIST::HANNING);
    int   window_size_exp = 10;
    float overlap_ratio   = 0.5f;
};

struct WaveformEncodeRequest {
    std::span<const float>            pcm;
    int                               channel_count       = 0;
    int                               y_pixels            = 0;
    int                               pcm_per_pixel       = 0;
    int                               x_pixels_per_image  = 0;
    int                               compression_level   = -1;
    int                               worker_thread_count = 0;
    std::optional<WaveformStftConfig> stft;
};

Array
EncodeWaveformWebps(const WaveformEncodeRequest &request);

}

} // namespace godot
