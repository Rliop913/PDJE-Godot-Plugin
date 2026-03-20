#include "EditorWrapper.hpp"
#include "jsonWrapper.hpp"
#include "pdje_util_common.hpp"
#include "variant/dictionary.hpp"

using namespace godot;

void
EditorWrapper::Init(std::shared_ptr<editorObject> refobj, PDJE *refengine)
{
    edit   = refobj;
    engine = refengine;
}

bool
EditorWrapper::AddLine(Ref<PDJE_EDITOR_ARG> arg)
{

    if (edit == nullptr)
        return false;
    switch (arg->useFlag) {
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::FLAG_NULL:
        return false;
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::NOTE:
        return edit->AddLine<EDIT_ARG_NOTE>(arg->note.value());
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::MUSIC:
        return edit->AddLine<EDIT_ARG_MUSIC>(arg->music.value());
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::MIX:
        return edit->AddLine<EDIT_ARG_MIX>(arg->mix.value());
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::KV:
        return edit->AddLine<EDIT_ARG_KEY_VALUE>(arg->kv.value());
    default:
        return false;
    }
    return false;
}

bool
EditorWrapper::EditMusicFirstBeat(String title, String firstBeat)
{

    if (edit == nullptr)
        return false;
    return edit->AddLine(GStrToCStr(title), GStrToCStr(firstBeat));
}

int
EditorWrapper::deleteLine(Ref<PDJE_EDITOR_ARG> obj,
                          bool                 skipType_if_mix_obj,
                          bool                 skipDetail_if_mix_obj)
{

    if (edit == nullptr)
        return -1;
    switch (obj->useFlag) {
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::FLAG_NULL:
        return 0;
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::NOTE:
        return edit->deleteLine<EDIT_ARG_NOTE>(obj->note.value());
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::MUSIC:
        return edit->deleteLine<EDIT_ARG_MUSIC>(obj->music.value());
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::MIX:
        return edit->deleteLine(
            obj->mix.value(), skipType_if_mix_obj, skipDetail_if_mix_obj);
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::KV:
        return edit->deleteLine<EDIT_ARG_KEY_VALUE>(obj->kv.value());
    default:
        return -2;
    }
}

String
EditorWrapper::render(String trackTitle)
{

    if (edit == nullptr)
        return "editor is null";
    if (engine == nullptr)
        return "engine is null";
    UNSANITIZED render_msg;
    if (edit->render(
            GStrToCStr(trackTitle), *engine->DBROOT.get(), render_msg)) {
        Flag_is_rendered = true;
        return "RENDER COMPLETE";
    } else {
        return CStrToGStr(render_msg);
    }
}

Ref<PlayerWrapper>
EditorWrapper::demoPlayInit(unsigned int frameBufferSize, String trackTitle)
{

    if (edit == nullptr) {
        critlog("failed to push to root db. editor is null");
        return {};
    }
    if (engine == nullptr) {
        critlog("failed to push to root db. engine is null");
        return {};
    }
    auto ref = Ref<PlayerWrapper>(memnew(PlayerWrapper));
    std::shared_ptr<audioPlayer> player;
    edit->demoPlayInit(player, frameBufferSize, GStrToCStr(trackTitle));
    ref->Init(player, engine);
    return ref;
}

bool
EditorWrapper::pushTrackToRootDB(String trackTitleToPush)
{

    if (edit == nullptr) {
        critlog("failed to push to root db. editor is null");
        return false;
    }
    if (engine == nullptr) {
        critlog("failed to push to root db. engine is null");
        return false;
    }
    if (!Flag_is_rendered) {
        critlog("failed to push to root db. it is not rendered");
        return false;
    }
    return edit->pushToRootDB(*engine->DBROOT.get(),
                              GStrToCStr(trackTitleToPush));
}

bool
EditorWrapper::pushToRootDB(String musicTitle, String musicComposer)
{

    if (edit == nullptr) {

        critlog("failed to push to root db. editor is null");
        return false;
    }
    if (engine == nullptr) {

        critlog("failed to push to root db. engine is null");
        return false;
    }
    if (!Flag_is_rendered) {

        critlog("failed to push to root db. it is not rendered");
        return false;
    }
    return edit->pushToRootDB(*engine->DBROOT.get(),
                              GStrToCStr(musicTitle),
                              GStrToCStr(musicComposer));
}

bool
EditorWrapper::getMixDatas()
{

    if (edit == nullptr)
        return false;

    edit->getAll<EDIT_ARG_MIX>([this](const EDIT_ARG_MIX &margs) {
        call_deferred("emit_signal",
                      "pdje_editor_get_mix_data",
                      static_cast<int>(margs.type),
                      margs.ID,
                      static_cast<int>(margs.details),
                      CStrToGStr(margs.first),
                      CStrToGStr(margs.second),
                      CStrToGStr(margs.third),
                      static_cast<int>(margs.beat),
                      static_cast<int>(margs.subBeat),
                      static_cast<int>(margs.separate),
                      static_cast<int>(margs.Ebeat),
                      static_cast<int>(margs.EsubBeat),
                      static_cast<int>(margs.Eseparate));
    });
    return true;
}

bool
EditorWrapper::getMusicBpmDatas()
{

    if (edit == nullptr)
        return false;

    edit->getAll<EDIT_ARG_MUSIC>([this](const EDIT_ARG_MUSIC &margs) {
        call_deferred("emit_signal",
                      "pdje_editor_get_music_bpm_data",
                      CStrToGStr(margs.musicName),
                      static_cast<int>(margs.arg.beat),
                      static_cast<int>(margs.arg.subBeat),
                      static_cast<int>(margs.arg.separate),
                      CStrToGStr(margs.arg.bpm));
    });
    return true;
}

bool
EditorWrapper::getNoteDatas()
{

    if (edit == nullptr)
        return false;

    edit->getAll<EDIT_ARG_NOTE>([this](const EDIT_ARG_NOTE &margs) {
        call_deferred("emit_signal",
                      "pdje_editor_get_note_data",
                      CStrToGStr(margs.Note_Type),
                      static_cast<int>(margs.Note_Detail),
                      CStrToGStr(margs.first),
                      CStrToGStr(margs.second),
                      CStrToGStr(margs.third),
                      static_cast<int>(margs.beat),
                      static_cast<int>(margs.subBeat),
                      static_cast<int>(margs.separate),
                      static_cast<int>(margs.Ebeat),
                      static_cast<int>(margs.EsubBeat),
                      static_cast<int>(margs.Eseparate),
                      static_cast<int>(margs.railID));
    });
    return true;
}

bool
EditorWrapper::getKeyValueDatas()
{

    if (edit == nullptr)
        return false;
    edit->getAll<EDIT_ARG_KEY_VALUE>([this](const EDIT_ARG_KEY_VALUE &margs) {
        call_deferred("emit_signal",
                      "pdje_editor_get_key_value_data",
                      CStrToGStr(margs.first),
                      CStrToGStr(margs.second));
    });
    return true;
}

Dictionary
EditorWrapper::getAll()
{

    if (edit == nullptr)
        return Dictionary();
    Dictionary result;
    Array      mixs;

    edit->getAll<EDIT_ARG_MIX>([&mixs](const EDIT_ARG_MIX &margs) {
        Dictionary mixline;
        switch (margs.type) {
        case TypeEnum::FILTER:
            mixline[PDJE_JSON_TYPE] = "FILTER";
            break;
        case TypeEnum::EQ:
            mixline[PDJE_JSON_TYPE] = "EQ";
            break;
        case TypeEnum::DISTORTION:
            mixline[PDJE_JSON_TYPE] = "DISTORTION";
            break;
        case TypeEnum::CONTROL:
            mixline[PDJE_JSON_TYPE] = "CONTROL";
            break;
        case TypeEnum::VOL:
            mixline[PDJE_JSON_TYPE] = "VOL";
            break;
        case TypeEnum::LOAD:
            mixline[PDJE_JSON_TYPE] = "LOAD";
            break;
        case TypeEnum::UNLOAD:
            mixline[PDJE_JSON_TYPE] = "UNLOAD";
            break;
        case TypeEnum::BPM_CONTROL:
            mixline[PDJE_JSON_TYPE] = "BPM_CONTROL";
            break;
        case TypeEnum::ECHO:
            mixline[PDJE_JSON_TYPE] = "ECHO";
            break;
        case TypeEnum::OSC_FILTER:
            mixline[PDJE_JSON_TYPE] = "OSC_FILTER";
            break;
        case TypeEnum::FLANGER:
            mixline[PDJE_JSON_TYPE] = "FLANGER";
            break;
        case TypeEnum::PHASER:
            mixline[PDJE_JSON_TYPE] = "PHASER";
            break;
        case TypeEnum::TRANCE:
            mixline[PDJE_JSON_TYPE] = "TRANCE";
            break;
        case TypeEnum::PANNER:
            mixline[PDJE_JSON_TYPE] = "PANNER";
            break;
        case TypeEnum::BATTLE_DJ:
            mixline[PDJE_JSON_TYPE] = "BATTLE_DJ";
            break;
        case TypeEnum::ROLL:
            mixline[PDJE_JSON_TYPE] = "ROLL";
            break;
        case TypeEnum::COMPRESSOR:
            mixline[PDJE_JSON_TYPE] = "COMPRESSOR";
            break;
        case TypeEnum::ROBOT:
            mixline[PDJE_JSON_TYPE] = "ROBOT";
            break;
        default:
            mixline[PDJE_JSON_TYPE] = "UNKNOWN";
            break;
        };

        switch (margs.details) {

        case DetailEnum::HIGH:
            mixline[PDJE_JSON_DETAILS] = "HIGH";
            break;
        case DetailEnum::MID:
            mixline[PDJE_JSON_DETAILS] = "MID";
            break;
        case DetailEnum::LOW:
            mixline[PDJE_JSON_DETAILS] = "LOW";
            break;
        case DetailEnum::PAUSE:
            mixline[PDJE_JSON_DETAILS] = "PAUSE";
            break;
        case DetailEnum::CUE:
            mixline[PDJE_JSON_DETAILS] = "CUE";
            break;
        case DetailEnum::TRIM:
            mixline[PDJE_JSON_DETAILS] = "TRIM";
            break;
        case DetailEnum::FADER:
            mixline[PDJE_JSON_DETAILS] = "FADER";
            break;
        case DetailEnum::TIME_STRETCH:
            mixline[PDJE_JSON_DETAILS] = "TIME_STRETCH";
            break;
        case DetailEnum::SPIN:
            mixline[PDJE_JSON_DETAILS] = "SPIN";
            break;
        case DetailEnum::PITCH:
            mixline[PDJE_JSON_DETAILS] = "PITCH";
            break;
        case DetailEnum::REV:
            mixline[PDJE_JSON_DETAILS] = "REV";
            break;
        case DetailEnum::SCRATCH:
            mixline[PDJE_JSON_DETAILS] = "SCRATCH";
            break;
        case DetailEnum::BSCRATCH:
            mixline[PDJE_JSON_DETAILS] = "BSCRATCH";
            break;
        default:
            mixline[PDJE_JSON_DETAILS] = "UNKNOWN";
            break;
        }
        mixline[PDJE_JSON_ID]        = margs.ID;
        mixline[PDJE_JSON_FIRST]     = CStrToGStr(margs.first);
        mixline[PDJE_JSON_SECOND]    = CStrToGStr(margs.second);
        mixline[PDJE_JSON_THIRD]     = CStrToGStr(margs.third);
        mixline[PDJE_JSON_BEAT]      = static_cast<int>(margs.beat);
        mixline[PDJE_JSON_SUBBEAT]   = static_cast<int>(margs.subBeat);
        mixline[PDJE_JSON_SEPARATE]  = static_cast<int>(margs.separate);
        mixline[PDJE_JSON_EBEAT]     = static_cast<int>(margs.Ebeat);
        mixline[PDJE_JSON_ESUBBEAT]  = static_cast<int>(margs.EsubBeat);
        mixline[PDJE_JSON_ESEPARATE] = static_cast<int>(margs.Eseparate);
        mixs.push_back(mixline);
    });
    result["mixDatas"] = mixs;
    Array musics;
    edit->getAll<EDIT_ARG_MUSIC>([&musics](const EDIT_ARG_MUSIC &margs) {
        Dictionary musicline;
        musicline[PDJE_JSON_TITLE]    = CStrToGStr(margs.musicName);
        musicline[PDJE_JSON_BEAT]     = static_cast<int>(margs.arg.beat);
        musicline[PDJE_JSON_SUBBEAT]  = static_cast<int>(margs.arg.subBeat);
        musicline[PDJE_JSON_BPM]      = CStrToGStr(margs.arg.bpm);
        musicline[PDJE_JSON_SEPARATE] = static_cast<int>(margs.arg.separate);
        musics.push_back(musicline);
    });
    result["musicDatas"] = musics;

    Array notes;
    edit->getAll<EDIT_ARG_NOTE>([&notes](const EDIT_ARG_NOTE &margs) {
        Dictionary noteline;

        noteline[PDJE_JSON_NOTE_TYPE]   = CStrToGStr(margs.Note_Type);
        noteline[PDJE_JSON_NOTE_DETAIL] = static_cast<int>(margs.Note_Detail);
        noteline[PDJE_JSON_FIRST]       = CStrToGStr(margs.first);
        noteline[PDJE_JSON_SECOND]      = CStrToGStr(margs.second);
        noteline[PDJE_JSON_THIRD]       = CStrToGStr(margs.third);
        noteline[PDJE_JSON_BEAT]        = static_cast<int>(margs.beat);
        noteline[PDJE_JSON_SUBBEAT]     = static_cast<int>(margs.subBeat);
        noteline[PDJE_JSON_SEPARATE]    = static_cast<int>(margs.separate);
        noteline[PDJE_JSON_EBEAT]       = static_cast<int>(margs.Ebeat);
        noteline[PDJE_JSON_ESUBBEAT]    = static_cast<int>(margs.EsubBeat);
        noteline[PDJE_JSON_ESEPARATE]   = static_cast<int>(margs.Eseparate);
        noteline[PDJE_JSON_RAILID]      = static_cast<int>(margs.railID);
        notes.push_back(noteline);
    });
    result["noteDatas"] = notes;

    Dictionary keyValues;
    edit->getAll<EDIT_ARG_KEY_VALUE>(
        [&keyValues](const EDIT_ARG_KEY_VALUE &margs) {
            keyValues[CStrToGStr(margs.first)] = CStrToGStr(margs.second);
        });
    result["keyValues"] = keyValues;
    return result;
}
