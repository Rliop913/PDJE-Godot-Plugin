#pragma once

#include "InputLine.hpp"
#include "Input_State.hpp"
#include "PDJE_Input.hpp"
#include "variant/array.hpp"
#include "variant/dictionary.hpp"
#include <godot_cpp/classes/node.hpp>

namespace godot {
class PDJE_Input_Module : public Node {
    GDCLASS(PDJE_Input_Module, Node)

  private:
    PDJE_Input input_module;

  protected:
    static void
    _bind_methods();

  public:
    enum INPUT_STATE {
        DEVICE_CONFIG_STATE = 0,
        INPUT_LOOP_READY,
        INPUT_LOOP_RUNNING,
        DEAD
    };
    bool
    Init();

    bool
    Config(Array devices);

    Array
    GetDevs();

    bool
    Kill();

    bool
    Run();

    INPUT_STATE
    GetState();

    Ref<InputLine>
    PullOutDataLine();

    PDJE_Input_Module();
    ~PDJE_Input_Module();

    void
    _ready() override;
    void
    _process(double delta) override;
};
} // namespace godot