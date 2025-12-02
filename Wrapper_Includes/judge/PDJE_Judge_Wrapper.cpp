#include "PDJE_Judge_Wrapper.hpp"

using namespace godot;

void
PDJE_Judge_Module::_bind_methods()
{
    ADD_SIGNAL(MethodInfo("pdje_judge_miss_signal",
                          PropertyInfo(Variant::DICTIONARY, "missed_list")));

    ADD_SIGNAL(MethodInfo("pdje_judge_use_signal",
                          PropertyInfo(Variant::STRING, "rail_id"),
                          PropertyInfo(Variant::BOOL, "is_pressed"),
                          PropertyInfo(Variant::BOOL, "is_late"),
                          PropertyInfo(Variant::STRING, "time_diff")));

    ADD_SIGNAL(MethodInfo("pdje_judge_custom_mouse_parse_signal",
                          PropertyInfo(Variant::STRING, "microsecond"),
                        PropertyInfo(Variant::ARRAY, "found_events"),
                        PropertyInfo(Variant::STRING, "rail_id"),
                        PropertyInfo(Variant::INT, "X"),
                        PropertyInfo(Variant::INT, "Y"),
                        PropertyInfo(Variant::INT, "AXIS_ENUM")
                        
                    ));
}
bool
PDJE_Judge_Module::AddDataLines(Ref<PDJE_Input_Module> input,
                                Ref<PDJE_Wrapper>      core)
{
    if (input.is_valid() && core.is_valid()) {
        judge_module.inits.coreline  = core->PullOutRawCoreLine();
        judge_module.inits.inputline = input->PullOutRawDataLine();
        if (judge_module.inits.coreline->maxCursor &&
            judge_module.inits.inputline->input_arena) {
            return true;
        } else {
            print_error("PDJE_Judge_Module: AddDataLines Failed. - input "
                        "module or core module does not have valid data line.");
            return false;
        }
    }
    print_error("PDJE_Judge_Module: AddDataLines Failed.- input module or core "
                "module is not valid.");
    return false;
}

void
PDJE_Judge_Module::SetRule(int  use_range_half_us,
                           int  miss_range_half_us,
                           int  useloop_sleep_time_ms,
                           int  missloop_sleep_time_ms,
                           bool enable_keyboard_signal,
                           bool enable_mouse_signal,
                           bool enable_custom_mouse_signal)
{
    judge_module.inits.SetEventRule(
        { .miss_range_microsecond = static_cast<uint64_t>(miss_range_half_us),
          .use_range_microsecond  = static_cast<uint64_t>(use_range_half_us) });

    PDJE_JUDGE::MISS_CALLBACK               missed;
    PDJE_JUDGE::USE_CALLBACK                used;
    PDJE_JUDGE::MOUSE_CUSTOM_PARSE_CALLBACK mouse_parse;
    if (!enable_keyboard_signal) {
        missed = [](std::unordered_map<uint64_t, PDJE_JUDGE::NOTE_VEC> misses) {
            return;
        };
    } else {
        missed =
            [&](std::unordered_map<uint64_t, PDJE_JUDGE::NOTE_VEC> misses) {
                Dictionary missed_list;
                for (const auto &obj : misses) {
                    Array datas;
                    for (const auto &obj_meta : obj.second) {
                        Dictionary meta;
                        meta["TYPE"]   = CStrToGStr(obj_meta.type);
                        meta["DETAIL"] = obj_meta.detail;
                        meta["ISDOWN"] = obj_meta.isDown;
                        meta["MICROSECOND"] =
                            String::num_uint64(obj_meta.microsecond);
                        meta["FIRST"]  = CStrToGStr(obj_meta.first);
                        meta["SECOND"] = CStrToGStr(obj_meta.second);
                        meta["THIRD"]  = CStrToGStr(obj_meta.third);
                        datas.push_back(meta);
                    }

                    missed_list[String::num_uint64(obj.first)] = datas;
                }
                call_deferred(
                    "emit_signal", "pdje_judge_miss_signal", missed_list);
            };
    }
    if (!enable_mouse_signal) {
        used = [](uint64_t railid, bool Pressed, bool IsLate, uint64_t diff) {
            return;
        };
    } else {
        used = [&](uint64_t railid, bool Pressed, bool IsLate, uint64_t diff) {
            call_deferred("emit_signal",
                          "pdje_judge_use_signal",
                          String::num_uint64(railid),
                          Pressed,
                          IsLate,
                          String::num_uint64(diff));
        };
    }
    if (!enable_custom_mouse_signal) {
        mouse_parse = [](uint64_t                      microSecond,
                         const PDJE_JUDGE::P_NOTE_VEC &found_events,
                         uint64_t                      railID,
                         int                           x,
                         int                           y,
                         PDJE_Mouse_Axis_Type          axis_type) { return; };
    } else {
        mouse_parse = [&](uint64_t                      microSecond,
                          const PDJE_JUDGE::P_NOTE_VEC &found_events,
                          uint64_t                      railID,
                          int                           x,
                          int                           y,
                          PDJE_Mouse_Axis_Type          axis_type) {
            Array Founds;
            for (const auto &fevent : found_events) {
                Dictionary meta;
                meta["TYPE"]        = CStrToGStr(fevent->type);
                meta["DETAIL"]      = fevent->detail;
                meta["ISDOWN"]      = fevent->isDown;
                meta["MICROSECOND"] = String::num_uint64(fevent->microsecond);
                meta["FIRST"]       = CStrToGStr(fevent->first);
                meta["SECOND"]      = CStrToGStr(fevent->second);
                meta["THIRD"]       = CStrToGStr(fevent->third);
                meta["USED"]        = fevent->used;

                Founds.push_back(meta);
            }

            call_deferred("emit_signal",
                          "pdje_judge_custom_mouse_parse_signal",
                          String::num_uint64(microSecond),
                          Founds,
                          String::num_uint64(railID),
                          x,
                          y,
                          static_cast<int>(axis_type));
        };
    }
    judge_module.inits.SetCustomEvents(
        { .missed_event       = missed,
          .used_event         = used,
          .custom_mouse_parse = mouse_parse,
          .use_event_sleep_time =
              std::chrono::milliseconds(useloop_sleep_time_ms),
          .miss_event_sleep_time =
              std::chrono::milliseconds(missloop_sleep_time_ms) });
}
PDJE_Judge_Module::PDJE_Judge_Module()
{
}
PDJE_Judge_Module::~PDJE_Judge_Module()
{
}