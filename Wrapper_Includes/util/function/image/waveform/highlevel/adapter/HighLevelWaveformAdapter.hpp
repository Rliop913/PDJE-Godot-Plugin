#pragma once

namespace godot {

class Array;
class Dictionary;
class PDJE_Wrapper;
class String;

namespace pdje_high_level_util::waveform {

Array
SoundToWaveform(PDJE_Wrapper     *core_api,
                const String     &keyvalue_db_path,
                const Dictionary &music_item,
                int               pcm_per_pixel,
                int               width,
                int               height,
                int               start_index,
                int               end_index);

}

} // namespace godot
