#pragma once

#include "EnumWrapper.hpp"
#include "PDJE_interface.hpp"
#include <godot_cpp/classes/ref.hpp>

namespace godot {

class FXArgWrapper : public RefCounted {
    GDCLASS(FXArgWrapper, RefCounted);

  private:
    std::optional<ARGSETTER_WRAPPER> refwrap;

  protected:
    static void
    _bind_methods();

  public:
    void
    Init(FXControlPanel *refobj);
    Array
    GetFXArgKeys(int fx);
    bool
    SetFXArg(int fx, String key, double arg);
    FXArgWrapper()  = default;
    ~FXArgWrapper() = default;
};

}; // namespace godot