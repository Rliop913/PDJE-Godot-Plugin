#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class PDJE_HighLevelUtilAPI : public Node {
    GDCLASS(PDJE_HighLevelUtilAPI, Node)

  protected:
    static void
    _bind_methods();

  public:
    Array
    SoundToWaveform(Object    *core_api,
                    String     keyvalue_db_path,
                    Dictionary music_item,
                    int        pcm_per_pixel,
                    int        width       = 256,
                    int        height      = 4096,
                    int        start_index = 0,
                    int        end_index   = -1);

    PDJE_HighLevelUtilAPI();
    ~PDJE_HighLevelUtilAPI();

    void
    _ready() override;
};

} // namespace godot
