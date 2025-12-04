#pragma once
#include "PDJE_Core_Wrapper.hpp"
#include "PDJE_Input_Wrapper.hpp"
#include "PDJE_Judge.hpp"
#include <godot_cpp/classes/node.hpp>

namespace godot {
class PDJE_Judge_Module : public Node {
    GDCLASS(PDJE_Judge_Module, Node)

  private:
    PDJE_JUDGE::JUDGE judge_module;

  protected:
    static void
    _bind_methods();

  public:
    bool
    AddDataLines(PDJE_Input_Module *input, PDJE_Wrapper *core);

    bool
    DeviceAdd(Dictionary device_list,
              int        PDJE_KEY_CODE,
              int        offset_microsecond,
              int        MatchRail_id);

    void
    SetRule(int  use_range_half_us,
            int  miss_range_half_us,
            int  useloop_sleep_time_ms,
            int  missloop_sleep_time_ms,
            bool enable_custom_mouse_signal);

    bool
    SetNotes(PDJE_Wrapper *core, String track_title);
    bool
    StartJudge();
    void
    EndJudge();

    PDJE_Judge_Module();
    ~PDJE_Judge_Module();

    // void
    // _ready() override;
};
} // namespace godot