#pragma once

namespace godot {

class Array;
class Dictionary;
class Object;
class String;

namespace pdje_high_level_util::waveform {

Array
SoundToWaveform(Object           *core_api,
                const String     &keyvalue_db_path,
                const Dictionary &music_item,
                int               pcm_per_pixel,
                int               width,
                int               height,
                int               start_index,
                int               end_index);

}

} // namespace godot
