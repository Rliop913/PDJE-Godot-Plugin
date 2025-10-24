#pragma once

#include <godot_cpp/classes/ref.hpp>

#include "PDJE_Input_DataLine.hpp"
#include "classes/ref_counted.hpp"
#include "classes/wrapped.hpp"
#include "variant/dictionary.hpp"

namespace godot {
class InputLine : public RefCounted {
    GDCLASS(InputLine, RefCounted)
  private:
    PDJE_INPUT_DATA_LINE input_data;

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