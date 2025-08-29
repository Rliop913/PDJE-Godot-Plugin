#pragma once

#include <godot_cpp/classes/ref.hpp>

#include "PDJE_interface.hpp"

namespace godot {

class CoreLine : public RefCounted {
    GDCLASS(CoreLine, RefCounted)
  private:
    PDJE_CORE_DATA_LINE core_data;

  protected:
    static void
    _bind_methods();

  public:
    void
    Init(const PDJE_CORE_DATA_LINE &coreDataLine);

    int64_t
    GetUsedFrame();
    int64_t
    GetNowCursor();
    int64_t
    GetMaxCursor();

    PackedFloat32Array
    GetPreRenderedFrames();

    CoreLine()  = default;
    ~CoreLine() = default;
};
} // namespace godot