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
    ClassDB::bind_method(D_METHOD("AddDataLines", "input", "core"),
                         &PDJE_Judge_Module::AddDataLines);
    ClassDB::bind_method(D_METHOD("DeviceAdd",
                                  "device_list",
                                  "PDJE_KEY_CODE",
                                  "offset_microsecond",
                                  "MatchRail_id"),
                         &PDJE_Judge_Module::DeviceAdd);
    ClassDB::bind_method(D_METHOD("SetRule",
                                  "use_range_half_us",
                                  "miss_range_half_us",
                                  "useloop_sleep_time_ms",
                                  "missloop_sleep_time_ms",
                                  "enable_custom_mouse_signal"),
                         &PDJE_Judge_Module::SetRule);
    ClassDB::bind_method(D_METHOD("SetNotes", "core", "track_title"),
                         &PDJE_Judge_Module::SetNotes);
    ClassDB::bind_method(D_METHOD("StartJudge"),
                         &PDJE_Judge_Module::StartJudge);
    ClassDB::bind_method(D_METHOD("EndJudge"), &PDJE_Judge_Module::EndJudge);
}
bool
PDJE_Judge_Module::AddDataLines(PDJE_Input_Module *input, PDJE_Wrapper *core)
{

    if (input != nullptr && core != nullptr) {
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
                           bool enable_custom_mouse_signal)
{
    judge_module.inits.SetEventRule(
        { .miss_range_microsecond = static_cast<uint64_t>(miss_range_half_us),
          .use_range_microsecond  = static_cast<uint64_t>(use_range_half_us) });

    PDJE_JUDGE::MISS_CALLBACK               missed;
    PDJE_JUDGE::USE_CALLBACK                used;
    PDJE_JUDGE::MOUSE_CUSTOM_PARSE_CALLBACK mouse_parse;

    missed = [&](std::unordered_map<uint64_t, PDJE_JUDGE::NOTE_VEC> misses) {
        Dictionary missed_list;
        for (const auto &obj : misses) {
            Array datas;
            for (const auto &obj_meta : obj.second) {
                Dictionary meta;
                meta["TYPE"]        = CStrToGStr(obj_meta.type);
                meta["DETAIL"]      = obj_meta.detail;
                meta["ISDOWN"]      = obj_meta.isDown;
                meta["MICROSECOND"] = String::num_uint64(obj_meta.microsecond);
                meta["FIRST"]       = CStrToGStr(obj_meta.first);
                meta["SECOND"]      = CStrToGStr(obj_meta.second);
                meta["THIRD"]       = CStrToGStr(obj_meta.third);
                datas.push_back(meta);
            }

            missed_list[String::num_uint64(obj.first)] = datas;
        }
        call_deferred("emit_signal", "pdje_judge_miss_signal", missed_list);
    };

    used = [&](uint64_t railid, bool Pressed, bool IsLate, uint64_t diff) {
        call_deferred("emit_signal",
                      "pdje_judge_use_signal",
                      String::num_uint64(railid),
                      Pressed,
                      IsLate,
                      String::num_uint64(diff));
    };

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

bool
PDJE_Judge_Module::SetNotes(PDJE_Wrapper *core, String track_title)
{
    if (core == nullptr) {
        print_error("core is not valid.");
        return false;
    }
    auto searched = core->engine->SearchTrack(GStrToCStr(track_title));
    if(searched.empty()){
        print_error(track_title, "doesn't have any tracks.");
        return false;
    }
    auto track    = searched.front();
    OBJ_SETTER_CALLBACK osc = [this](std::string        note_type,
                                     uint16_t           note_detail,
                                     std::string        first_arg,
                                     std::string        second_arg,
                                     std::string        third_arg,
                                     unsigned long long y_pos_start,
                                     unsigned long long y_pos_end,
                                     uint64_t           railID) {
        judge_module.inits.NoteObjectCollector(note_type,
                                               note_detail,
                                               first_arg,
                                               second_arg,
                                               third_arg,
                                               y_pos_start,
                                               y_pos_end,
                                               railID);
    };
    if (!core->engine->GetNoteObjects(track, osc)) {
        print_error("failed to add note objects.");
        return false;
    }
    return core->GetNoteObjects(track_title);
}

bool
PDJE_Judge_Module::DeviceAdd(Dictionary devData,
                             int        PDJE_KEY_CODE,
                             int        offset_microsecond,
                             int        MatchRail_id)
{
    DeviceData dev;
    if (devData.has("device_specific_id") && devData.has("name") &&
        devData.has("type")) {
        dev.device_specific_id = GStrToCStr(devData["device_specific_id"]);
        dev.Name               = GStrToCStr(devData["name"]);
        String ttype           = devData["type"];
        if (ttype == "KEYBOARD") {
            dev.Type = PDJE_Dev_Type::KEYBOARD;
        } else if (ttype == "MOUSE") {
            dev.Type = PDJE_Dev_Type::MOUSE;
        } else {
            dev.Type = PDJE_Dev_Type::UNKNOWN;
        }
        if (dev.device_specific_id != "" && dev.Name != "" &&
            dev.Type != PDJE_Dev_Type::UNKNOWN) {
            judge_module.inits.SetRail(dev,
                                       static_cast<BITMASK>(PDJE_KEY_CODE),
                                       static_cast<int64_t>(offset_microsecond),
                                       static_cast<uint64_t>(MatchRail_id));

            return true;
        } else {
            print_error(
                "device specific, name is empty or device type is invalid.");
        }
    } else {
        print_error("devData must have [device_specific_id], [name], [type] as "
                    "a Dictionary Key.");
    }
    return false;
}

bool
PDJE_Judge_Module::StartJudge()
{
    switch (judge_module.Start()) {
    case PDJE_JUDGE::JUDGE_STATUS::CORE_LINE_IS_MISSING:
        print_error("core data line is missing.");
        return false;
    case PDJE_JUDGE::JUDGE_STATUS::EVENT_RULE_IS_EMPTY:
        print_error("pdje rule(Event Side) is empty");
        return false;
    case PDJE_JUDGE::JUDGE_STATUS::INPUT_LINE_IS_MISSING:
        print_error("input date line is missing.");
        return false;
    case PDJE_JUDGE::JUDGE_STATUS::INPUT_RULE_IS_EMPTY:
        print_error("pdje rule(Input Side) is empty");
        return false;
    case PDJE_JUDGE::JUDGE_STATUS::NOTE_OBJECT_IS_MISSING:
        print_error("note object is missing. please set note datas.");
        return false;
    case PDJE_JUDGE::JUDGE_STATUS::OK:
        return true;
    default:
        print_error(
            "RUNTIME LOGIC ERROR ON START JUDGE. PLEASE REPORT TO DEV.");
        return false;
    }
}

void
PDJE_Judge_Module::EndJudge()
{
    judge_module.End();
}

PDJE_Judge_Module::PDJE_Judge_Module()
{
}
PDJE_Judge_Module::~PDJE_Judge_Module()
{
}