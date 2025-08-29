#pragma once

#include <godot_cpp/classes/ref.hpp>

#include "PDJE_Utils.hpp"
#include "PDJE_interface.hpp"
namespace godot {

class PDJE_EDITOR_ARG : public RefCounted {
    GDCLASS(PDJE_EDITOR_ARG, RefCounted)
  private:
  protected:
    static void
    _bind_methods();

  public:
    std::optional<EDIT_ARG_NOTE>      note;
    std::optional<EDIT_ARG_KEY_VALUE> kv;
    std::optional<EDIT_ARG_MIX>       mix;
    std::optional<EDIT_ARG_MUSIC>     music;
    enum FLAG_WHAT_TO_USE { FLAG_NULL = 0, NOTE, KV, MIX, MUSIC };
    FLAG_WHAT_TO_USE useFlag = FLAG_NULL;
    PDJE_EDITOR_ARG()        = default;
    ~PDJE_EDITOR_ARG()       = default;
    enum EDITOR_TYPE_LIST {
        FILTER = 0,
        EQ,
        DISTORTION,
        CONTROL,
        VOL,
        LOAD,
        UNLOAD,
        BPM_CONTROL,
        ECHO,
        OSC_FILTER,
        FLANGER,
        PHASER,
        TRANCE,
        PANNER,
        BATTLE_DJ,
        ROLL,
        COMPRESSOR,
        ROBOT
    };

    enum EDITOR_DETAIL_LIST {
        HIGH = 0,
        MID,
        LOW,
        PAUSE,
        CUE,
        TRIM,
        FADER,
        TIME_STRETCH,
        SPIN,
        PITCH,
        REV,
        SCRATCH,
        BSCRATCH
    };

    void
    InitNoteArg(String Note_Type,
                String Note_Detail,
                String first,
                String second,
                String third,
                int    bar,
                int    beat,
                int    separate,
                int    Ebar,
                int    Ebeat,
                int    Eseparate);
    void
    InitMusicArg(String musicName, String bpm, int bar, int beat, int separate);

    void
    InitMixArg(int    enum_editor_type,
               int    enum_editor_details,
               int    ID,
               String first,
               String second,
               String third,
               int    bar,
               int    beat,
               int    separate,
               int    Ebar,
               int    Ebeat,
               int    Eseparate);
    void
    InitKeyValueArg(String key, String value);
};
} // namespace godot