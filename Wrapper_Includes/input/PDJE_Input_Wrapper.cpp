#include "PDJE_Input_Wrapper.hpp"
#include "InputLine.hpp"
#include "Input_State.hpp"
#include "PDJE_Input.hpp"
#include "PDJE_Input_Device_Data.hpp"
#include "PDJE_Utils.hpp"
#include "classes/ref.hpp"
#include "core/binder_common.hpp"
#include "core/class_db.hpp"
#include "core/memory.hpp"
#include "variant/array.hpp"
#include "variant/dictionary.hpp"
#include "variant/typed_array.hpp"
#include "variant/variant.hpp"
#include "PDJE_LOG_SETTER.hpp"
using namespace godot;

VARIANT_ENUM_CAST(PDJE_Input_Module::INPUT_STATE);

void
PDJE_Input_Module::_bind_methods()
{
    BIND_ENUM_CONSTANT(INPUT_STATE::INPUT_LOOP_READY);
    BIND_ENUM_CONSTANT(INPUT_STATE::DEVICE_CONFIG_STATE);
    BIND_ENUM_CONSTANT(INPUT_STATE::DEAD);
    BIND_ENUM_CONSTANT(INPUT_STATE::INPUT_LOOP_RUNNING);
    ClassDB::bind_method(D_METHOD("Init"), &PDJE_Input_Module::Init);
    ClassDB::bind_method(D_METHOD("Config", "devices"),
                         &PDJE_Input_Module::Config);
    ClassDB::bind_method(D_METHOD("Kill"), &PDJE_Input_Module::Kill);
    ClassDB::bind_method(D_METHOD("Run"), &PDJE_Input_Module::Run);
    ClassDB::bind_method(D_METHOD("GetState"), &PDJE_Input_Module::GetState);
    ClassDB::bind_method(D_METHOD("PullOutDataLine"),
                         &PDJE_Input_Module::PullOutDataLine);
    ClassDB::bind_method(D_METHOD("GetDevs"),
                         &PDJE_Input_Module::GetDevs);
    
}

PDJE_Input_Module::PDJE_Input_Module()
{
}

PDJE_Input_Module::~PDJE_Input_Module()
{
}

Array
PDJE_Input_Module::GetDevs()
{
    DEV_LIST  devs;
    try
    {
        devs = input_module.GetDevs();
    }
    catch(const std::exception& e)
    {
        godot::print_line(CStrToGStr( e.what()));
        critlog("failed to get devs. Why:");
        critlog(e.what());
    }
    Array out;
    for (const auto &d : devs) {
        Dictionary devtemp;
        devtemp["device_specific_id"] = CStrToGStr(d.device_specific_id);
        devtemp["name"]               = CStrToGStr(d.Name);
        switch (d.Type) {
        case PDJE_Dev_Type::KEYBOARD:
            devtemp["type"] = "KEYBOARD";
            break;
        case PDJE_Dev_Type::MOUSE:
            devtemp["type"] = "MOUSE";
            break;
        case PDJE_Dev_Type::MIDI:
            devtemp["type"] = "MIDI";
            break;
        case PDJE_Dev_Type::HID:
            devtemp["type"] = "HID";
            break;
        case PDJE_Dev_Type::UNKNOWN:
            devtemp["type"] = "";
            break;
        default:
            devtemp["type"] = "";
            break;
        }
        
        if (d.Name != "" && d.device_specific_id != "" && d.Type != PDJE_Dev_Type::UNKNOWN) {
            out.push_back(devtemp);
        }
    }
    return out;
}

bool
PDJE_Input_Module::Config(Array devices)
{
    std::vector<DeviceData> devs;
    for (int i = 0; i < devices.size(); ++i) {
        if (devices[i].get_type() == Variant::DICTIONARY) {
            Dictionary dict = devices[i];
            DeviceData dd;
            dd.device_specific_id = GStrToCStr(dict["device_specific_id"]);
            dd.Name               = GStrToCStr(dict["name"]);
            String ttype            = dict["type"];
            if (ttype == "KEYBOARD") {
                dd.Type = PDJE_Dev_Type::KEYBOARD;
            } else if (ttype == "MOUSE") {
                dd.Type = PDJE_Dev_Type::MOUSE;
            } else if (ttype == "MIDI") {
                dd.Type = PDJE_Dev_Type::MIDI;
            } else if (ttype == "HID") {
                dd.Type = PDJE_Dev_Type::HID;
            } else {
                dd.Type = PDJE_Dev_Type::UNKNOWN;
            }
            if (dd.device_specific_id != "" && dd.Name != "" &&
                dd.Type != PDJE_Dev_Type::UNKNOWN) {
                devs.push_back(dd);
            } else {
                return false;
            }
        }
    }
    return input_module.Config(devs);
}

Ref<InputLine>
PDJE_Input_Module::PullOutDataLine()
{
    auto ref = Ref<InputLine>(memnew(InputLine));
    if (input_module.GetState() == PDJE_INPUT_STATE::INPUT_LOOP_READY ||
        input_module.GetState() == PDJE_INPUT_STATE::INPUT_LOOP_RUNNING) {
        auto line = input_module.PullOutDataLine();
        ref->Init(line);
    }
    return ref;
}

bool
PDJE_Input_Module::Init()
{
    return input_module.Init();
}

bool
PDJE_Input_Module::Kill()
{
    return input_module.Kill();
}

bool
PDJE_Input_Module::Run()
{
    return input_module.Run();
}

PDJE_Input_Module::INPUT_STATE
PDJE_Input_Module::GetState()
{
    return static_cast<PDJE_Input_Module::INPUT_STATE>(input_module.GetState());
}


void
PDJE_Input_Module::_ready()
{

}