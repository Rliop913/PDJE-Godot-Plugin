#pragma once

#include <godot_cpp/classes/ref.hpp>

namespace godot {

class EnumWrapper : public RefCounted {
    GDCLASS(EnumWrapper, RefCounted)
  private:
  protected:
    static void
    _bind_methods();

  public:
    enum PDJE_FX_LIST {
        COMPRESSOR = 0,
        DISTORTION,
        ECHO,
        EQ,
        FILTER,
        FLANGER,
        OCSFILTER,
        PANNER,
        PHASER,
        ROBOT,
        ROLL,
        TRANCE,
        VOL
    };
    EnumWrapper()  = default;
    ~EnumWrapper() = default;
};

} // namespace godot