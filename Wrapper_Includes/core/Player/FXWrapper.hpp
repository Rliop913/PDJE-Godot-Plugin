#pragma once

#include "FXArgWrapper.hpp"
#include "PDJE_interface.hpp"
#include <godot_cpp/classes/ref.hpp>
namespace godot {

class FXWrapper : public RefCounted {
    GDCLASS(FXWrapper, RefCounted)
  private:
    FXControlPanel *fxpanel = nullptr;

  protected:
    static void
    _bind_methods();

  public:
    FXWrapper() = default;
    void
    Init(FXControlPanel *refobj);
    bool
    CheckFXOn();
    bool
    FX_ON_OFF(int fx, bool onoff);
    Ref<FXArgWrapper>
    GetArgSetter();
    ~FXWrapper() = default;
};

}; // namespace godot