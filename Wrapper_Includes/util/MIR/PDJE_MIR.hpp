#pragma once

#include "STFT_Parallel.hpp"
#include "variant/packed_byte_array.hpp"
#include "variant/packed_color_array.hpp"
#include "variant/packed_float32_array.hpp"
#include "variant/typed_array.hpp"
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <memory>
#include <optional>

namespace godot {

class PDJE_KeyValueDB;
class PDJE_StftResult;
class PDJE_Wrapper;

// namespace pdje_domain_internal {
// struct StftState;
// }

class PDJE_MIR : public Node {
    GDCLASS(PDJE_MIR, Node)

  private:
    std::optional<std::unique_ptr<PDJE_PARALLEL::STFT>> stft_ptr;
    std::mutex s_mir_waveform_cache_write_mutex;
    std::mutex mir_stft_cache_write_mutex;

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
    SoundToWaveform(PDJE_Wrapper    *core_api,
                    PDJE_KeyValueDB *cache_db,
                    String           musicTitle,
                    String           composer,
                    float            bpm,
                    int              pcm_per_pixel,
                    int              width       = 4096,
                    int              height      = 256,
                    int              start_index = 0,
                    int              end_index   = -1);

    Array
    SoundToRGBWaveform(PDJE_Wrapper    *core_api,
                       PDJE_KeyValueDB *cache_db,
                       String           musicTitle,
                       String           composer,
                       float            bpm,
                       int              pcm_per_pixel,
                       int              width           = 4096,
                       int              height          = 256,
                       int              start_index     = 0,
                       int              end_index       = -1,
                       int              target_window   = HANNING,
                       int              window_size_exp = 10,
                       float            overlap_ratio   = 0.5f);

    PackedColorArray
    STFT_MUSIC(PDJE_Wrapper    *core_api,
               PDJE_KeyValueDB *cache_db,
               String           musicTitle,
               String           composer,
               float            bpm,
               int              target_window   = HANNING,
               int              window_size_exp = 10,
               float            overlap_ratio   = 0.5f);

    TypedArray<Ref<PDJE_StftResult>>
    STFT_PCM_DATA(PackedFloat32Array pcm,
                  int                channel_count,
                  int                target_window     = HANNING,
                  int                window_size_exp   = 10,
                  float              overlap_ratio     = 0.5f,
                  bool               toPower           = true,
                  bool               to_bin            = true,
                  bool               normalize_min_max = true,
                  bool               mel_scale         = true,
                  bool               to_db             = true,
                  bool               to_rgb            = true);

    PDJE_MIR();
    ~PDJE_MIR();

    void
    _ready() override;
};

} // namespace godot
