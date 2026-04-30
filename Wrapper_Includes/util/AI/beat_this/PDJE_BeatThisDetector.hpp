#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <memory>

namespace PDJE_UTIL::ai {
class BeatThisDetector;
}

namespace godot {

class PDJE_BeatThisResult;
class PDJE_Wrapper;

class PDJE_BeatThisDetector : public RefCounted {
    GDCLASS(PDJE_BeatThisDetector, RefCounted)

  private:
    std::unique_ptr<PDJE_UTIL::ai::BeatThisDetector> detector_;

  protected:
    static void
    _bind_methods();

  public:
    PDJE_BeatThisDetector();
    ~PDJE_BeatThisDetector();

    bool
    Init(String model_path);
    bool
    IsInitialized() const;
    String
    GetModelPath() const;

    Ref<PDJE_BeatThisResult>
    DetectPCM(PackedFloat32Array pcm, int channel_count, int sample_rate);
    Ref<PDJE_BeatThisResult>
    DetectMusic(PDJE_Wrapper *core_api,
                String        music_title,
                String        composer,
                double        bpm = -1.0);
};

} // namespace godot
