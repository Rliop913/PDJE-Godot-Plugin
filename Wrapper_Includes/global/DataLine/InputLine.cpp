#include "InputLine.hpp"
#include "Input_State.hpp"
#include "PDJE_Input_Wrapper.hpp"
#include "PDJE_Rule.hpp"
#include "PDJE_Utils.hpp"
#include "core/binder_common.hpp"
#include "core/class_db.hpp"
#include "core/property_info.hpp"
#include "variant/dictionary.hpp"
#include "variant/string.hpp"
#include "variant/variant.hpp"

using namespace godot;

VARIANT_ENUM_CAST(PDJE_KEY);
using namespace PDJE_JUDGE;
VARIANT_ENUM_CAST(DEVICE_MOUSE_EVENT);

void
InputLine::_bind_methods()
{
    using enum PDJE_KEY;
    using enum DEVICE_MOUSE_EVENT;

    // keyboard enum bind
    BIND_ENUM_CONSTANT(F_1);
    BIND_ENUM_CONSTANT(F_2);
    BIND_ENUM_CONSTANT(F_3);
    BIND_ENUM_CONSTANT(F_4);
    BIND_ENUM_CONSTANT(F_5);
    BIND_ENUM_CONSTANT(F_6);
    BIND_ENUM_CONSTANT(F_7);
    BIND_ENUM_CONSTANT(F_8);
    BIND_ENUM_CONSTANT(F_9);
    BIND_ENUM_CONSTANT(F_10);
    BIND_ENUM_CONSTANT(F_11);
    BIND_ENUM_CONSTANT(F_12);
    BIND_ENUM_CONSTANT(D1);
    BIND_ENUM_CONSTANT(D2);
    BIND_ENUM_CONSTANT(D3);
    BIND_ENUM_CONSTANT(D4);
    BIND_ENUM_CONSTANT(D5);
    BIND_ENUM_CONSTANT(D6);
    BIND_ENUM_CONSTANT(D7);
    BIND_ENUM_CONSTANT(D8);
    BIND_ENUM_CONSTANT(D9);
    BIND_ENUM_CONSTANT(D0);
    BIND_ENUM_CONSTANT(Q);
    BIND_ENUM_CONSTANT(W);
    BIND_ENUM_CONSTANT(E);
    BIND_ENUM_CONSTANT(R);
    BIND_ENUM_CONSTANT(T);
    BIND_ENUM_CONSTANT(Y);
    BIND_ENUM_CONSTANT(U);
    BIND_ENUM_CONSTANT(I);
    BIND_ENUM_CONSTANT(O);
    BIND_ENUM_CONSTANT(P);
    BIND_ENUM_CONSTANT(A);
    BIND_ENUM_CONSTANT(S);
    BIND_ENUM_CONSTANT(D);
    BIND_ENUM_CONSTANT(F);
    BIND_ENUM_CONSTANT(G);
    BIND_ENUM_CONSTANT(H);
    BIND_ENUM_CONSTANT(J);
    BIND_ENUM_CONSTANT(K);
    BIND_ENUM_CONSTANT(L);
    BIND_ENUM_CONSTANT(Z);
    BIND_ENUM_CONSTANT(X);
    BIND_ENUM_CONSTANT(C);
    BIND_ENUM_CONSTANT(V);
    BIND_ENUM_CONSTANT(B);
    BIND_ENUM_CONSTANT(N);
    BIND_ENUM_CONSTANT(M);
    BIND_ENUM_CONSTANT(KP_1);
    BIND_ENUM_CONSTANT(KP_2);
    BIND_ENUM_CONSTANT(KP_3);
    BIND_ENUM_CONSTANT(KP_4);
    BIND_ENUM_CONSTANT(KP_5);
    BIND_ENUM_CONSTANT(KP_6);
    BIND_ENUM_CONSTANT(KP_7);
    BIND_ENUM_CONSTANT(KP_8);
    BIND_ENUM_CONSTANT(KP_9);
    BIND_ENUM_CONSTANT(KP_0);
    BIND_ENUM_CONSTANT(KP_DOT);
    BIND_ENUM_CONSTANT(KP_ENTER);
    BIND_ENUM_CONSTANT(KP_PLUS);
    BIND_ENUM_CONSTANT(KP_NUMLOCK);
    BIND_ENUM_CONSTANT(KP_SLASH);
    BIND_ENUM_CONSTANT(KP_STAR);
    BIND_ENUM_CONSTANT(KP_MINUS);
    BIND_ENUM_CONSTANT(ENTER);
    BIND_ENUM_CONSTANT(ESC);
    BIND_ENUM_CONSTANT(BACKSPACE);
    BIND_ENUM_CONSTANT(TAB);
    BIND_ENUM_CONSTANT(SPACE);
    BIND_ENUM_CONSTANT(CAPSLK);
    BIND_ENUM_CONSTANT(LEFT);
    BIND_ENUM_CONSTANT(RIGHT);
    BIND_ENUM_CONSTANT(UP);
    BIND_ENUM_CONSTANT(DOWN);
    BIND_ENUM_CONSTANT(LCTRL);
    BIND_ENUM_CONSTANT(RCTRL);
    BIND_ENUM_CONSTANT(LALT);
    BIND_ENUM_CONSTANT(RALT);
    BIND_ENUM_CONSTANT(LSHIFT);
    BIND_ENUM_CONSTANT(RSHIFT);
    BIND_ENUM_CONSTANT(MINUS);
    BIND_ENUM_CONSTANT(EQUAL);
    BIND_ENUM_CONSTANT(LBRACKET);
    BIND_ENUM_CONSTANT(RBRACKET);
    BIND_ENUM_CONSTANT(BACKSLASH);
    BIND_ENUM_CONSTANT(SLASH);
    BIND_ENUM_CONSTANT(SEMICOLON);
    BIND_ENUM_CONSTANT(APOSTROPHE);
    BIND_ENUM_CONSTANT(GRAVE);
    BIND_ENUM_CONSTANT(COMMA);
    BIND_ENUM_CONSTANT(PERIOD);
    BIND_ENUM_CONSTANT(NONUS_BACKSLASH);
    BIND_ENUM_CONSTANT(SP_PRINT_SCREEN);
    BIND_ENUM_CONSTANT(SP_SCROLL_LOCK);
    BIND_ENUM_CONSTANT(SP_INSERT);
    BIND_ENUM_CONSTANT(SP_HOME);
    BIND_ENUM_CONSTANT(SP_END);
    BIND_ENUM_CONSTANT(SP_DELETE);
    BIND_ENUM_CONSTANT(SP_PAGE_UP);
    BIND_ENUM_CONSTANT(SP_PAGE_DOWN);
    BIND_ENUM_CONSTANT(UNKNOWN);

    // mouse enum bind
    BIND_ENUM_CONSTANT(PDJE_BTN_EX);
    BIND_ENUM_CONSTANT(PDJE_BTN_SIDE);
    BIND_ENUM_CONSTANT(PDJE_BTN_M);
    BIND_ENUM_CONSTANT(PDJE_BTN_R);
    BIND_ENUM_CONSTANT(PDJE_BTN_L);
    BIND_ENUM_CONSTANT(PDJE_WHEEL_X);
    BIND_ENUM_CONSTANT(PDJE_WHEEL_Y);
    BIND_ENUM_CONSTANT(PDJE_AXIS_MOVE);

    ADD_SIGNAL(MethodInfo("pdje_input_keyboard_signal",
                          PropertyInfo(Variant::STRING, "device_id"),
                          PropertyInfo(Variant::STRING, "device_name"),
                          PropertyInfo(Variant::STRING, "microsecond_string"),
                          PropertyInfo(Variant::INT, "keyboard_key"),
                          PropertyInfo(Variant::BOOL, "isPressed")));

    ADD_SIGNAL(MethodInfo("pdje_input_mouse_signal",
                          PropertyInfo(Variant::STRING, "device_id"),
                          PropertyInfo(Variant::STRING, "device_name"),
                          PropertyInfo(Variant::STRING, "microsecond_string"),
                          PropertyInfo(Variant::INT, "L_btn"),
                          PropertyInfo(Variant::INT, "R_btn"),
                          PropertyInfo(Variant::INT, "wheel_btn"),
                          PropertyInfo(Variant::INT, "side_btn"),
                          PropertyInfo(Variant::INT, "ex_btn"),
                          PropertyInfo(Variant::BOOL, "is_wheel_YAxis"),
                          PropertyInfo(Variant::INT, "wheel_move"),
                          PropertyInfo(Variant::STRING, "mouse_axis_type"),
                          PropertyInfo(Variant::INT, "x"),
                          PropertyInfo(Variant::INT, "y")));

    ADD_SIGNAL(MethodInfo("pdje_midi_input_signal",
                          PropertyInfo(Variant::STRING, "port_name"),
                          PropertyInfo(Variant::STRING, "input_type"),
                          PropertyInfo(Variant::INT, "channel"),
                          PropertyInfo(Variant::INT, "position"),
                          PropertyInfo(Variant::INT, "value"),
                          PropertyInfo(Variant::STRING, "microsecond_string")));

    ClassDB::bind_method(D_METHOD("emit_input_signal"),
                         &InputLine::emit_input_signal);
}

void
InputLine::Init(const PDJE_INPUT_DATA_LINE &inputDataLine)
{

    input_data = inputDataLine;
}

void
InputLine::ParseMouse(mouse_events &mev, const uint16_t bit_mask)
{

    if (bit_mask & PDJE_MOUSE_L_BTN_DOWN) {
        mev.L_btn = -1;

    } else if (bit_mask & PDJE_MOUSE_L_BTN_UP) {
        mev.L_btn = 1;
    }
    if (bit_mask & PDJE_MOUSE_R_BTN_DOWN) {
        mev.R_btn = -1;
    } else if (bit_mask & PDJE_MOUSE_R_BTN_UP) {
        mev.R_btn = 1;
    }
    if (bit_mask & PDJE_MOUSE_M_BTN_DOWN) {
        mev.wheel_btn = -1;
    } else if (bit_mask & PDJE_MOUSE_M_BTN_UP) {
        mev.wheel_btn = 1;
    }
    if (bit_mask & PDJE_MOUSE_SIDE_BTN_DOWN) {
        mev.side_btn = -1;
    } else if (bit_mask & PDJE_MOUSE_SIDE_BTN_UP) {
        mev.side_btn = 1;
    }
    if (bit_mask & PDJE_MOUSE_EX_BTN_DOWN) {
        mev.ex_btn = -1;
    } else if (bit_mask & PDJE_MOUSE_EX_BTN_UP) {
        mev.ex_btn = 1;
    }
    if (bit_mask & PDJE_MOUSE_XWHEEL) {
        mev.is_wheel_YAxis = false;
    } else if (bit_mask & PDJE_MOUSE_YWHEEL) {
        mev.is_wheel_YAxis = true;
    }
}

void
InputLine::ParseInputSignal(const PDJE_Input_Log &log)
{

    switch (log.type) {
    case PDJE_Dev_Type::KEYBOARD:
        call_deferred("emit_signal",
                      "pdje_input_keyboard_signal",
                      CStrToGStr(std::string(log.id, log.id_len)),
                      CStrToGStr(std::string(log.name, log.name_len)),
                      String::num_uint64(log.microSecond),
                      log.event.keyboard.k,
                      log.event.keyboard.pressed);
        break;
    case PDJE_Dev_Type::MOUSE: {
        mouse_events temp_event;
        ParseMouse(temp_event, log.event.mouse.button_type);
        String AxisType = "";
        switch (log.event.mouse.axis_type) {
        case PDJE_Mouse_Axis_Type::REL:
            AxisType = "REL";
            break;
        case PDJE_Mouse_Axis_Type::ABS:
            AxisType = "ABS";
            break;
        case PDJE_Mouse_Axis_Type::VIRTUAL_DESKTOP_ABS:
            AxisType = "VIRTUAL_DESKTOP_ABS";
            break;
        default:
            AxisType = "ERROR";
            break;
        }
        call_deferred("emit_signal",
                      "pdje_input_mouse_signal",
                      CStrToGStr(std::string(log.id, log.id_len)),
                      CStrToGStr(std::string(log.name, log.name_len)),
                      String::num_uint64(log.microSecond),
                      temp_event.L_btn,
                      temp_event.R_btn,
                      temp_event.wheel_btn,
                      temp_event.side_btn,
                      temp_event.ex_btn,
                      temp_event.is_wheel_YAxis,
                      log.event.mouse.wheel_move,
                      AxisType,
                      log.event.mouse.x,
                      log.event.mouse.y);
        break;
    }
    case PDJE_Dev_Type::UNKNOWN:
        break;
    default:
        break;
    }
}

void
InputLine::ParseMIDIInputSignal(const PDJE_MIDI::MIDI_EV &midilog)
{
    String midi_type_string;
    switch (midilog.type) {
    case static_cast<uint8_t>(libremidi::message_type::NOTE_ON):
        midi_type_string = "NOTE_ON";
        break;
    case static_cast<uint8_t>(libremidi::message_type::NOTE_OFF):
        midi_type_string = "NOTE_OFF";
        break;
    case static_cast<uint8_t>(libremidi::message_type::PITCH_BEND):
        midi_type_string = "PITCH_BEND";
        break;
    case static_cast<uint8_t>(libremidi::message_type::CONTROL_CHANGE):
        midi_type_string = "CONTROL_CHANGE";
        break;
    case static_cast<uint8_t>(libremidi::message_type::AFTERTOUCH):
        midi_type_string = "AFTERTOUCH";
        break;
    case static_cast<uint8_t>(libremidi::message_type::POLY_PRESSURE):
        midi_type_string = "POLY_PRESSURE";
        break;
    default:
        return;
    }
    call_deferred(
        "emit_signal",
        "pdje_midi_input_signal",
        CStrToGStr(std::string(midilog.port_name, midilog.port_name_len)),
        midi_type_string,
        static_cast<int>(midilog.ch),
        static_cast<int>(midilog.pos),
        static_cast<int>(midilog.value),
        String::num_uint64(midilog.highres_time));
}

void
InputLine::emit_input_signal()
{
    if (input_data.input_arena) {
        input_data.input_arena->Receive();
        auto got = input_data.input_arena->datas;

        for (auto &i : got) {
            ParseInputSignal(i);
        }
    }
    if (input_data.midi_datas) {
        auto got = input_data.midi_datas->Get();
        for (const auto &i : *got) {
            ParseMIDIInputSignal(i);
        }
    }
    if (input_data.input_arena == nullptr && input_data.midi_datas == nullptr) {
        godot::print_line(
            "failed to emit input signal. input line is not initialized.");
        return;
    }
}