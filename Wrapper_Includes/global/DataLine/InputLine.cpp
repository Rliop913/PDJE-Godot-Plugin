#include "InputLine.hpp"
#include "Input_State.hpp"
#include "PDJE_Input_Wrapper.hpp"
#include "PDJE_Utils.hpp"
#include "core/binder_common.hpp"
#include "core/class_db.hpp"
#include "core/property_info.hpp"
#include "variant/dictionary.hpp"
#include "variant/string.hpp"
#include "variant/variant.hpp"

using namespace godot;

VARIANT_ENUM_CAST(PDJE_KEY);

void
InputLine::_bind_methods()
{
    BIND_ENUM_CONSTANT(PDJE_KEY::F_1);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_2);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_3);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_4);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_5);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_6);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_7);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_8);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_9);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_10);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_11);
    BIND_ENUM_CONSTANT(PDJE_KEY::F_12);
    BIND_ENUM_CONSTANT(PDJE_KEY::D1);
    BIND_ENUM_CONSTANT(PDJE_KEY::D2);
    BIND_ENUM_CONSTANT(PDJE_KEY::D3);
    BIND_ENUM_CONSTANT(PDJE_KEY::D4);
    BIND_ENUM_CONSTANT(PDJE_KEY::D5);
    BIND_ENUM_CONSTANT(PDJE_KEY::D6);
    BIND_ENUM_CONSTANT(PDJE_KEY::D7);
    BIND_ENUM_CONSTANT(PDJE_KEY::D8);
    BIND_ENUM_CONSTANT(PDJE_KEY::D9);
    BIND_ENUM_CONSTANT(PDJE_KEY::D0);
    BIND_ENUM_CONSTANT(PDJE_KEY::Q);
    BIND_ENUM_CONSTANT(PDJE_KEY::W);
    BIND_ENUM_CONSTANT(PDJE_KEY::E);
    BIND_ENUM_CONSTANT(PDJE_KEY::R);
    BIND_ENUM_CONSTANT(PDJE_KEY::T);
    BIND_ENUM_CONSTANT(PDJE_KEY::Y);
    BIND_ENUM_CONSTANT(PDJE_KEY::U);
    BIND_ENUM_CONSTANT(PDJE_KEY::I);
    BIND_ENUM_CONSTANT(PDJE_KEY::O);
    BIND_ENUM_CONSTANT(PDJE_KEY::P);
    BIND_ENUM_CONSTANT(PDJE_KEY::A);
    BIND_ENUM_CONSTANT(PDJE_KEY::S);
    BIND_ENUM_CONSTANT(PDJE_KEY::D);
    BIND_ENUM_CONSTANT(PDJE_KEY::F);
    BIND_ENUM_CONSTANT(PDJE_KEY::G);
    BIND_ENUM_CONSTANT(PDJE_KEY::H);
    BIND_ENUM_CONSTANT(PDJE_KEY::J);
    BIND_ENUM_CONSTANT(PDJE_KEY::K);
    BIND_ENUM_CONSTANT(PDJE_KEY::L);
    BIND_ENUM_CONSTANT(PDJE_KEY::Z);
    BIND_ENUM_CONSTANT(PDJE_KEY::X);
    BIND_ENUM_CONSTANT(PDJE_KEY::C);
    BIND_ENUM_CONSTANT(PDJE_KEY::V);
    BIND_ENUM_CONSTANT(PDJE_KEY::B);
    BIND_ENUM_CONSTANT(PDJE_KEY::N);
    BIND_ENUM_CONSTANT(PDJE_KEY::M);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_1);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_2);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_3);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_4);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_5);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_6);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_7);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_8);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_9);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_0);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_DOT);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_ENTER);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_PLUS);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_NUMLOCK);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_SLASH);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_STAR);
    BIND_ENUM_CONSTANT(PDJE_KEY::KP_MINUS);
    BIND_ENUM_CONSTANT(PDJE_KEY::ENTER);
    BIND_ENUM_CONSTANT(PDJE_KEY::ESC);
    BIND_ENUM_CONSTANT(PDJE_KEY::BACKSPACE);
    BIND_ENUM_CONSTANT(PDJE_KEY::TAB);
    BIND_ENUM_CONSTANT(PDJE_KEY::SPACE);
    BIND_ENUM_CONSTANT(PDJE_KEY::CAPSLK);
    BIND_ENUM_CONSTANT(PDJE_KEY::LEFT);
    BIND_ENUM_CONSTANT(PDJE_KEY::RIGHT);
    BIND_ENUM_CONSTANT(PDJE_KEY::UP);
    BIND_ENUM_CONSTANT(PDJE_KEY::DOWN);
    BIND_ENUM_CONSTANT(PDJE_KEY::LCTRL);
    BIND_ENUM_CONSTANT(PDJE_KEY::RCTRL);
    BIND_ENUM_CONSTANT(PDJE_KEY::LALT);
    BIND_ENUM_CONSTANT(PDJE_KEY::RALT);
    BIND_ENUM_CONSTANT(PDJE_KEY::LSHIFT);
    BIND_ENUM_CONSTANT(PDJE_KEY::RSHIFT);
    BIND_ENUM_CONSTANT(PDJE_KEY::MINUS);
    BIND_ENUM_CONSTANT(PDJE_KEY::EQUAL);
    BIND_ENUM_CONSTANT(PDJE_KEY::LBRACKET);
    BIND_ENUM_CONSTANT(PDJE_KEY::RBRACKET);
    BIND_ENUM_CONSTANT(PDJE_KEY::BACKSLASH);
    BIND_ENUM_CONSTANT(PDJE_KEY::SLASH);
    BIND_ENUM_CONSTANT(PDJE_KEY::SEMICOLON);
    BIND_ENUM_CONSTANT(PDJE_KEY::APOSTROPHE);
    BIND_ENUM_CONSTANT(PDJE_KEY::GRAVE);
    BIND_ENUM_CONSTANT(PDJE_KEY::COMMA);
    BIND_ENUM_CONSTANT(PDJE_KEY::PERIOD);
    BIND_ENUM_CONSTANT(PDJE_KEY::NONUS_BACKSLASH);
    BIND_ENUM_CONSTANT(PDJE_KEY::SP_PRINT_SCREEN);
    BIND_ENUM_CONSTANT(PDJE_KEY::SP_SCROLL_LOCK);
    BIND_ENUM_CONSTANT(PDJE_KEY::SP_INSERT);
    BIND_ENUM_CONSTANT(PDJE_KEY::SP_HOME);
    BIND_ENUM_CONSTANT(PDJE_KEY::SP_END);
    BIND_ENUM_CONSTANT(PDJE_KEY::SP_DELETE);
    BIND_ENUM_CONSTANT(PDJE_KEY::SP_PAGE_UP);
    BIND_ENUM_CONSTANT(PDJE_KEY::SP_PAGE_DOWN);
    BIND_ENUM_CONSTANT(PDJE_KEY::UNKNOWN);

    ADD_SIGNAL(MethodInfo("pdje_input_keyboard_signal",
                          PropertyInfo(Variant::STRING, "device_id"),
                          PropertyInfo(Variant::STRING, "microsecond_string"),
                          PropertyInfo(Variant::INT, "keyboard_key"),
                          PropertyInfo(Variant::BOOL, "isPressed")));

    ADD_SIGNAL(MethodInfo("pdje_input_mouse_signal",
                          PropertyInfo(Variant::STRING, "device_id"),
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

    ClassDB::bind_method(D_METHOD("get_id_name_list"),
                         &InputLine::get_id_name_list);
    ClassDB::bind_method(D_METHOD("emit_input_signal"),
                         &InputLine::emit_input_signal);
}

void
InputLine::Init(const PDJE_INPUT_DATA_LINE &inputDataLine)
{
    
        input_data = inputDataLine;
   
}

Dictionary
InputLine::get_id_name_list()
{
    
        if(input_data.id_name_conv == nullptr){
            print_line("input line is not initialized.");
            return Dictionary();
        }
        Dictionary out;
        for (const auto &i : (*input_data.id_name_conv)) {
            out[CStrToGStr(i.first)] = CStrToGStr(i.second);
        }
        return out;
   
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
InputLine::emit_input_signal()
{
    
        if(input_data.input_arena == nullptr){
            print_line("inputline is not initialized.");
            return;
        }
        auto got = input_data.input_arena->Get();
        
        for (const auto &log : *got) {
            switch (log.type) {
            case PDJE_Dev_Type::KEYBOARD:
                print_line("got keyboard", log.microSecond);
                call_deferred("emit_signal",
                              "pdje_input_keyboard_signal",
                              CStrToGStr(log.id),
                              String::num_uint64(log.microSecond),
                              log.event.keyboard.k,
                              log.event.keyboard.pressed);
                break;
            case PDJE_Dev_Type::MOUSE: {
                print_line("got mouse", log.microSecond);
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
                              CStrToGStr(log.id),
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
            case PDJE_Dev_Type::MIDI:
                break;
            case PDJE_Dev_Type::HID:

                break;
            case PDJE_Dev_Type::UNKNOWN:
                break;
            default:
                break;
            }
        }
    
}