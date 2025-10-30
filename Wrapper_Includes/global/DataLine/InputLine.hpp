#pragma once

#include <cstdint>
#include <godot_cpp/classes/node.hpp>
// #include <godot_cpp/classes/ref.hpp>

#include "PDJE_Input_DataLine.hpp"
#include "classes/ref_counted.hpp"
#include "classes/wrapped.hpp"
#include "variant/dictionary.hpp"

namespace godot {
class InputLine : public Node {
    GDCLASS(InputLine, Node)
  private:
    PDJE_INPUT_DATA_LINE input_data;
    struct mouse_events {
        int  L_btn          = 0;
        int  R_btn          = 0;
        int  wheel_btn      = 0;
        int  side_btn       = 0;
        int  ex_btn         = 0;
        bool is_wheel_YAxis = true;
    };

    void
    ParseMouse(mouse_events &mev, const uint16_t bit_mask);

  protected:
    static void
    _bind_methods();

  public:
    void
    Init(const PDJE_INPUT_DATA_LINE &inputDataLine);

    Dictionary
    get_id_name_list();

    void
    emit_input_signal();

    InputLine()  = default;
    ~InputLine() = default;
};

} // namespace godot