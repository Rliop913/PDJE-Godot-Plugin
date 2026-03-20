#include "PDJE_HighLevelUtilAPI.hpp"

#include "PDJE_Core_Wrapper.hpp"
#include "util/function/image/waveform/highlevel/adapter/HighLevelWaveformAdapter.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void
PDJE_HighLevelUtilAPI::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("SoundToWaveform",
                                  "core_api",
                                  "keyvalue_db_path",
                                  "music_item",
                                  "pcm_per_pixel",
                                  "width",
                                  "height",
                                  "start_index",
                                  "end_index"),
                         &PDJE_HighLevelUtilAPI::SoundToWaveform,
                         DEFVAL(4096),
                         DEFVAL(256),
                         DEFVAL(0),
                         DEFVAL(-1));
}

Array
PDJE_HighLevelUtilAPI::SoundToWaveform(PDJE_Wrapper *core_api,
                                       String        keyvalue_db_path,
                                       Dictionary    music_item,
                                       int           pcm_per_pixel,
                                       int           width,
                                       int           height,
                                       int           start_index,
                                       int           end_index)
{
    return pdje_high_level_util::waveform::SoundToWaveform(core_api,
                                                           keyvalue_db_path,
                                                           music_item,
                                                           pcm_per_pixel,
                                                           width,
                                                           height,
                                                           start_index,
                                                           end_index);
}

PDJE_HighLevelUtilAPI::PDJE_HighLevelUtilAPI() = default;

PDJE_HighLevelUtilAPI::~PDJE_HighLevelUtilAPI() = default;

void
PDJE_HighLevelUtilAPI::_ready()
{
}
