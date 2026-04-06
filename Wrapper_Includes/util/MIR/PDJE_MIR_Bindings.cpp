#include "util/MIR/PDJE_MIR.hpp"

#include "PDJE_Core_Wrapper.hpp"
#include "util/MIR/PDJE_StftResult.hpp"
#include "util/db/keyvalue/PDJE_KeyValueDB.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

VARIANT_ENUM_CAST(PDJE_MIR::STFT_WINDOW_LIST);

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
                                  "music_title",
                                  "composer",
                                  "bpm",
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
    ClassDB::bind_method(D_METHOD("SoundToRGBWaveform",
                                  "core_api",
                                  "cache_db",
                                  "music_title",
                                  "composer",
                                  "bpm",
                                  "pcm_per_pixel",
                                  "width",
                                  "height",
                                  "start_index",
                                  "end_index",
                                  "target_window",
                                  "window_size_exp",
                                  "overlap_ratio"),
                         &PDJE_MIR::SoundToRGBWaveform,
                         DEFVAL(4096),
                         DEFVAL(256),
                         DEFVAL(0),
                         DEFVAL(-1),
                         DEFVAL(HANNING),
                         DEFVAL(10),
                         DEFVAL(0.5f));
    ClassDB::bind_method(D_METHOD("STFT_MUSIC",
                                  "core_api",
                                  "cache_db",
                                  "music_title",
                                  "composer",
                                  "bpm",
                                  "target_window",
                                  "window_size_exp",
                                  "overlap_ratio"),
                         &PDJE_MIR::STFT_MUSIC,
                         DEFVAL(HANNING),
                         DEFVAL(10),
                         DEFVAL(0.5f));
    ClassDB::bind_method(D_METHOD("STFT_PCM_DATA",
                                  "pcm",
                                  "channel_count",
                                  "target_window",
                                  "window_size_exp",
                                  "overlap_ratio",
                                  "toPower",
                                  "to_bin",
                                  "normalize_min_max",
                                  "mel_scale",
                                  "to_db",
                                  "to_rgb"),
                         &PDJE_MIR::STFT_PCM_DATA,
                         DEFVAL(HANNING),
                         DEFVAL(10),
                         DEFVAL(0.5f),
                         DEFVAL(true),
                         DEFVAL(true),
                         DEFVAL(true),
                         DEFVAL(true),
                         DEFVAL(true),
                         DEFVAL(true));
}

void
PDJE_MIR::_ready()
{
}
