#include "PDJE_Input_Wrapper.hpp"
#include "InputLine.hpp"
#include "Input_State.hpp"
#include "PDJE_Input.hpp"
#include "PDJE_Input_Device_Data.hpp"
#include "PDJE_LOG_SETTER.hpp"
#include "PDJE_Utils.hpp"
#include "classes/ref.hpp"
#include "core/binder_common.hpp"
#include "core/class_db.hpp"
#include "core/memory.hpp"
#include "variant/array.hpp"
#include "variant/dictionary.hpp"
#include "variant/typed_array.hpp"
#include "variant/variant.hpp"
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
    ClassDB::bind_method(D_METHOD("Config", "devices", "MIDIdevices"),
                         &PDJE_Input_Module::Config);
    ClassDB::bind_method(D_METHOD("Kill"), &PDJE_Input_Module::Kill);
    ClassDB::bind_method(D_METHOD("Run"), &PDJE_Input_Module::Run);
    ClassDB::bind_method(D_METHOD("GetState"), &PDJE_Input_Module::GetState);
    ClassDB::bind_method(D_METHOD("InitializeInputLine", "input_line"),
                         &PDJE_Input_Module::InitializeInputLine);
    ClassDB::bind_method(D_METHOD("GetDevs"), &PDJE_Input_Module::GetDevs);
    ClassDB::bind_method(D_METHOD("GetMIDIDevs"),
                         &PDJE_Input_Module::GetMIDIDevs);
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

    auto  devs = input_module.GetDevs();
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
        case PDJE_Dev_Type::UNKNOWN:
            devtemp["type"] = "";
            break;
        default:
            devtemp["type"] = "";
            break;
        }
        if (d.Name != "" && d.device_specific_id != "" &&
            d.Type != PDJE_Dev_Type::UNKNOWN) {
            out.push_back(devtemp);
        }
    }
    return out;
}

Array
PDJE_Input_Module::GetMIDIDevs()
{
    auto  devs = input_module.GetMIDIDevs();
    Array devlist;
    for (const auto &i : devs) {
        devlist.append(CStrToGStr(i.port_name));
    }
    return devlist;
}

bool
PDJE_Input_Module::Config(Array devices, Array MIDIdevices)
{

    std::vector<DeviceData> devs;
    for (int i = 0; i < devices.size(); ++i) {
        if (devices[i].get_type() == Variant::DICTIONARY) {
            Dictionary dict = devices[i];
            DeviceData dd;
            dd.device_specific_id = GStrToCStr(dict["device_specific_id"]);
            dd.Name               = GStrToCStr(dict["name"]);
            String ttype          = dict["type"];
            if (ttype == "KEYBOARD") {
                dd.Type = PDJE_Dev_Type::KEYBOARD;
            } else if (ttype == "MOUSE") {
                dd.Type = PDJE_Dev_Type::MOUSE;
            } else {
                dd.Type = PDJE_Dev_Type::UNKNOWN;
            }
            if (dd.device_specific_id != "" && dd.Name != "" &&
                dd.Type != PDJE_Dev_Type::UNKNOWN) {
                devs.push_back(dd);
            } else {
                continue;
            }
        }
    }
    auto                               midis = input_module.GetMIDIDevs();
    std::vector<libremidi::input_port> target_midis;
    if (!MIDIdevices.is_empty() && !midis.empty()) {
        for (auto &midi : midis) {
            bool is_matched = false;
            for (int i = 0; i < MIDIdevices.size(); ++i) {
                if (midi.port_name == GStrToCStr(MIDIdevices[i])) {
                    is_matched = true;
                    break;
                }
            }
            if (is_matched) {
                target_midis.push_back(midi);
            }
        }
    }
    return input_module.Config(devs, target_midis);
}

void
PDJE_Input_Module::InitializeInputLine(InputLine *input_line)
{

    input_line->Init(input_module.PullOutDataLine());
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

PDJE_INPUT_DATA_LINE
PDJE_Input_Module::PullOutRawDataLine()
{
    if (input_module.GetState() == PDJE_INPUT_STATE::DEAD) {
        print_error("pull out raw dataline from pdje input module. - input "
                    "module is not initialized.");
        return {};
    }
    return input_module.PullOutDataLine();
}

void
PDJE_Input_Module::_ready()
{
}