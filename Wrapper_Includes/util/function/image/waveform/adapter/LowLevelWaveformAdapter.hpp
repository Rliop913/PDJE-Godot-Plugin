#pragma once

namespace godot {

class Array;
class Dictionary;

namespace pdje_low_level_util::waveform {

Dictionary
EncodeWaveformWebps(const Array &pcm,
                    int          channel_count,
                    int          y_pixels,
                    int          pcm_per_pixel,
                    int          x_pixels_per_image,
                    int          compression_level,
                    int          worker_thread_count);

}

} // namespace godot
