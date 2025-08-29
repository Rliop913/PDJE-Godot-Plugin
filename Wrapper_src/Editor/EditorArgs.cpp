#include "EditorArgs.hpp"

using namespace godot;

VARIANT_ENUM_CAST(PDJE_EDITOR_ARG::EDITOR_TYPE_LIST);
VARIANT_ENUM_CAST(PDJE_EDITOR_ARG::EDITOR_DETAIL_LIST);

void
PDJE_EDITOR_ARG::_bind_methods()
{
    BIND_ENUM_CONSTANT(FILTER);
    BIND_ENUM_CONSTANT(EQ);
    BIND_ENUM_CONSTANT(DISTORTION);
    BIND_ENUM_CONSTANT(CONTROL);
    BIND_ENUM_CONSTANT(VOL);
    BIND_ENUM_CONSTANT(LOAD);
    BIND_ENUM_CONSTANT(UNLOAD);
    BIND_ENUM_CONSTANT(BPM_CONTROL);
    BIND_ENUM_CONSTANT(ECHO);
    BIND_ENUM_CONSTANT(OSC_FILTER);
    BIND_ENUM_CONSTANT(FLANGER);
    BIND_ENUM_CONSTANT(PHASER);
    BIND_ENUM_CONSTANT(TRANCE);
    BIND_ENUM_CONSTANT(PANNER);
    BIND_ENUM_CONSTANT(BATTLE_DJ);
    BIND_ENUM_CONSTANT(ROLL);
    BIND_ENUM_CONSTANT(COMPRESSOR);
    BIND_ENUM_CONSTANT(ROBOT);

    BIND_ENUM_CONSTANT(HIGH);
    BIND_ENUM_CONSTANT(MID);
    BIND_ENUM_CONSTANT(LOW);
    BIND_ENUM_CONSTANT(PAUSE);
    BIND_ENUM_CONSTANT(CUE);
    BIND_ENUM_CONSTANT(TRIM);
    BIND_ENUM_CONSTANT(FADER);
    BIND_ENUM_CONSTANT(TIME_STRETCH);
    BIND_ENUM_CONSTANT(SPIN);
    BIND_ENUM_CONSTANT(PITCH);
    BIND_ENUM_CONSTANT(REV);
    BIND_ENUM_CONSTANT(SCRATCH);
    BIND_ENUM_CONSTANT(BSCRATCH);
    ClassDB::bind_method(D_METHOD("InitNoteArg",
                                  "Note_Type",
                                  "Note_Detail",
                                  "first",
                                  "second",
                                  "third",
                                  "beat",
                                  "subBeat",
                                  "separate",
                                  "Ebeat",
                                  "EsubBeat",
                                  "Eseparate"),
                         &PDJE_EDITOR_ARG::InitNoteArg);

    ClassDB::bind_method(
        D_METHOD(
            "InitMusicArg", "musicName", "bpm", "beat", "subBeat", "separate"),
        &PDJE_EDITOR_ARG::InitMusicArg);
    ClassDB::bind_method(D_METHOD("InitMixArg",
                                  "enum_editor_type",
                                  "enum_editor_details",
                                  "ID",
                                  "first",
                                  "second",
                                  "third",
                                  "beat",
                                  "subBeat",
                                  "separate",
                                  "Ebeat",
                                  "EsubBeat",
                                  "Eseparate"),
                         &PDJE_EDITOR_ARG::InitMixArg);
    ClassDB::bind_method(D_METHOD("InitKeyValueArg", "key", "value"),
                         &PDJE_EDITOR_ARG::InitKeyValueArg);
}
void
PDJE_EDITOR_ARG::InitNoteArg(String Note_Type,
                             String Note_Detail,
                             String first,
                             String second,
                             String third,
                             int    beat,
                             int    subBeat,
                             int    separate,
                             int    Ebeat,
                             int    EsubBeat,
                             int    Eseparate)
{
    note.emplace();

    note->Note_Type   = GStrToCStr(Note_Type);
    note->Note_Detail = GStrToCStr(Note_Detail);
    note->first       = GStrToCStr(first);
    note->second      = GStrToCStr(second);
    note->third       = GStrToCStr(third);
    note->beat        = beat;
    note->subBeat     = subBeat;
    note->separate    = separate;
    note->Ebeat       = Ebeat;
    note->EsubBeat    = EsubBeat;
    note->Eseparate   = Eseparate;

    useFlag = NOTE;
}
void
PDJE_EDITOR_ARG::InitMusicArg(
    String musicName, String bpm, int beat, int subBeat, int separate)
{
    music.emplace();
    music->musicName    = GStrToCStr(musicName);
    music->arg.bpm      = GStrToCStr(bpm);
    music->arg.beat     = beat;
    music->arg.subBeat  = subBeat;
    music->arg.separate = separate;

    useFlag = MUSIC;
}

void
PDJE_EDITOR_ARG::InitMixArg(int    type,
                            int    details,
                            int    ID,
                            String first,
                            String second,
                            String third,
                            int    beat,
                            int    subBeat,
                            int    separate,
                            int    Ebeat,
                            int    EsubBeat,
                            int    Eseparate)
{
    mix.emplace();
    switch (type) {
    case FILTER:
        mix->type = TypeEnum::FILTER;
        break;
    case EQ:
        mix->type = TypeEnum::EQ;
        break;
    case DISTORTION:
        mix->type = TypeEnum::DISTORTION;
        break;
    case CONTROL:
        mix->type = TypeEnum::CONTROL;
        break;
    case VOL:
        mix->type = TypeEnum::VOL;
        break;
    case LOAD:
        mix->type = TypeEnum::LOAD;
        break;
    case UNLOAD:
        mix->type = TypeEnum::UNLOAD;
        break;
    case BPM_CONTROL:
        mix->type = TypeEnum::BPM_CONTROL;
        break;
    case ECHO:
        mix->type = TypeEnum::ECHO;
        break;
    case OSC_FILTER:
        mix->type = TypeEnum::OSC_FILTER;
        break;
    case FLANGER:
        mix->type = TypeEnum::FLANGER;
        break;
    case PHASER:
        mix->type = TypeEnum::PHASER;
        break;
    case TRANCE:
        mix->type = TypeEnum::TRANCE;
        break;
    case PANNER:
        mix->type = TypeEnum::PANNER;
        break;
    case BATTLE_DJ:
        mix->type = TypeEnum::BATTLE_DJ;
        break;
    case ROLL:
        mix->type = TypeEnum::ROLL;
        break;
    case COMPRESSOR:
        mix->type = TypeEnum::COMPRESSOR;
        break;
    case ROBOT:
        mix->type = TypeEnum::ROBOT;
        break;
    default:
        return;
    }

    switch (details) {
    case HIGH:
        mix->details = DetailEnum::HIGH;
        break;
    case MID:
        mix->details = DetailEnum::MID;
        break;
    case LOW:
        mix->details = DetailEnum::LOW;
        break;
    case PAUSE:
        mix->details = DetailEnum::PAUSE;
        break;
    case CUE:
        mix->details = DetailEnum::CUE;
        break;
    case TRIM:
        mix->details = DetailEnum::TRIM;
        break;
    case FADER:
        mix->details = DetailEnum::FADER;
        break;
    case TIME_STRETCH:
        mix->details = DetailEnum::TIME_STRETCH;
        break;
    case SPIN:
        mix->details = DetailEnum::SPIN;
        break;
    case PITCH:
        mix->details = DetailEnum::PITCH;
        break;
    case REV:
        mix->details = DetailEnum::REV;
        break;
    case SCRATCH:
        mix->details = DetailEnum::SCRATCH;
        break;
    case BSCRATCH:
        mix->details = DetailEnum::BSCRATCH;
        break;

    default:
        return;
    }
    mix->ID        = ID;
    mix->first     = GStrToCStr(first);
    mix->second    = GStrToCStr(second);
    mix->third     = GStrToCStr(third);
    mix->beat      = beat;
    mix->subBeat   = subBeat;
    mix->separate  = separate;
    mix->Ebeat     = Ebeat;
    mix->EsubBeat  = EsubBeat;
    mix->Eseparate = Eseparate;

    useFlag = MIX;
}

void
PDJE_EDITOR_ARG::InitKeyValueArg(String key, String value)
{
    kv.emplace();
    kv->first  = GStrToCStr(key);
    kv->second = GStrToCStr(value);
    useFlag    = KV;
}
