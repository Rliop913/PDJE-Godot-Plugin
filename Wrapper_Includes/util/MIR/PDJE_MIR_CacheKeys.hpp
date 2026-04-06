#pragma once

#include <godot_cpp/variant/string.hpp>

namespace godot::pdje_mir_internal::cache_keys {

inline String
BuildWaveformCacheKey(const String &cache_source_key,
                      int           width,
                      int           height,
                      int           pcm_per_pixel)
{
    return String("MIR|waveform|") + cache_source_key + String("|w:") +
           String::num_int64(width) + String("|h:") +
           String::num_int64(height) + String("|pp:") +
           String::num_int64(pcm_per_pixel);
}

inline String
BuildWaveformRgbCacheKey(const String &cache_source_key,
                         int           width,
                         int           height,
                         int           pcm_per_pixel,
                         int           target_window,
                         int           window_size_exp,
                         float         overlap_ratio)
{
    return String("MIR|waveform_rgb|") + cache_source_key + String("|w:") +
           String::num_int64(width) + String("|h:") +
           String::num_int64(height) + String("|pp:") +
           String::num_int64(pcm_per_pixel) + String("|window:") +
           String::num_int64(target_window) + String("|exp:") +
           String::num_int64(window_size_exp) + String("|overlap:") +
           String::num_real(overlap_ratio);
}

inline String
BuildStftMusicCacheKey(const String &cache_source_key,
                       int           target_window,
                       int           window_size_exp,
                       float         overlap_ratio)
{
    return String("MIR|stft_music|") + cache_source_key + String("|window:") +
           String::num_int64(target_window) + String("|exp:") +
           String::num_int64(window_size_exp) + String("|overlap:") +
           String::num_real(overlap_ratio);
}

} // namespace godot::pdje_mir_internal::cache_keys
