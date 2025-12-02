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

    // device init
    void
    SetRail(const DeviceData &devData,
            const BITMASK     DeviceKey,
            const int64_t     offset_microsecond,
            const uint64_t    MatchRail);
    // set judge rule
    void
    SetSignal();
    void
    SetEventRule();

  protected:
    static void
    _bind_methods();

  public:
    bool
    AddDataLines(Ref<PDJE_Input_Module> input, Ref<PDJE_Wrapper> core);

    void
    DeviceAdd(Array device_list,
              int   PDJE_KEY_CODE,
              int   offset_microsecond,
              int   MatchRail_id);

    void
    SetRule(int  use_range_half_us,
            int  miss_range_half_us,
            int  useloop_sleep_time_ms,
            int  missloop_sleep_time_ms,
            bool enable_keyboard_signal,
            bool enable_mouse_signal,
            bool enable_custom_mouse_signal);

    void
    SetNotes(String track_title);

    void
    StartJudge();
    void
    EndJudge();

    PDJE_Judge_Module();
    ~PDJE_Judge_Module();

    void
    _ready() override;
};
} // namespace godot