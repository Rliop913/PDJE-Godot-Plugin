#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>

#include <memory>

namespace godot {

class PDJE_KeyValueDB;
class PDJE_StftResult;
class PDJE_Wrapper;

namespace pdje_domain_internal {
struct StftState;
}

class PDJE_MIR : public Node {
    GDCLASS(PDJE_MIR, Node)

  private:
    std::unique_ptr<pdje_domain_internal::StftState> stft_state_;

  protected:
    static void
    _bind_methods();

  public:
    enum STFT_WINDOW_LIST {
        BLACKMAN = 0,
        BLACKMAN_HARRIS,
        BLACKMAN_NUTTALL,
        HANNING,
        NUTTALL,
        FLATTOP,
        GAUSSIAN,
        HAMMING,
        NONE
    };

    Array
    SoundToWaveform(PDJE_Wrapper   *core_api,
                    PDJE_KeyValueDB *cache_db,
                    Dictionary      music_item,
                    int             pcm_per_pixel,
                    int             width       = 4096,
                    int             height      = 256,
                    int             start_index = 0,
                    int             end_index   = -1);

    bool
    STFTCreate();

    bool
    STFTDestroy();

    Ref<PDJE_StftResult>
    STFT_PCM_DATA(Array pcm,
                  int   channel_count,
                  int   target_window   = HANNING,
                  int   window_size_exp = 10,
                  float overlap_ratio   = 0.5f,
                  bool  to_power        = false);

    PDJE_MIR();
    ~PDJE_MIR();

    void
    _ready() override;
};

} // namespace godot
