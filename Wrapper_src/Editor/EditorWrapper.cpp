#include "EditorWrapper.hpp"

using namespace godot;

void
EditorWrapper::Init(editorObject* refobj, PDJE* refengine)
{
    edit = refobj;
    engine = refengine;
    
}

bool
EditorWrapper::AddLine(Ref<PDJE_EDITOR_ARG> arg)
{
    if(edit == nullptr) return false;
    switch (arg->useFlag)
    {
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
EditorWrapper::EditMusicFirstBar(String title, String firstBar)
{
    if(edit == nullptr) return false;
    return
    edit->AddLine(
        GStrToCStr(title),
        GStrToCStr(firstBar)
    );
}

int 
EditorWrapper::deleteLine(
    Ref<PDJE_EDITOR_ARG> obj,
    bool skipType_if_mix_obj, 
    bool skipDetail_if_mix_obj)
{
    if(edit == nullptr) return 0;
    switch (obj->useFlag)
    {
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::FLAG_NULL:
        return 0;
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::NOTE:
        return edit->deleteLine<EDIT_ARG_NOTE>(obj->note.value());
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::MUSIC:
        return edit->deleteLine<EDIT_ARG_MUSIC>(obj->music.value());
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::MIX:
        return edit->deleteLine(obj->mix.value(), skipType_if_mix_obj, skipDetail_if_mix_obj);
    case PDJE_EDITOR_ARG::FLAG_WHAT_TO_USE::KV:
        return edit->deleteLine<EDIT_ARG_KEY_VALUE>(obj->kv.value());
    default:
        return 0;
    }
    return 0;
}

bool
EditorWrapper::render(String trackTitle)
{
    if(edit == nullptr) return false;
    if(engine == nullptr) return false;
    Flag_is_rendered = true;
    return
    edit->render(
        GStrToCStr(trackTitle),
        *engine->DBROOT.get()
    );
}

bool
EditorWrapper::demoPlayInit(unsigned int frameBufferSize, String trackTitle)
{
    if(edit == nullptr) return false;
    if(engine == nullptr) return false;
    edit->demoPlayInit(engine->player, frameBufferSize, GStrToCStr(trackTitle));
    return true;
}

bool
EditorWrapper::pushTrackToRootDB(String trackTitleToPush)
{
    if(edit == nullptr) return false;
    if(engine == nullptr) return false;
    if(!Flag_is_rendered) return false;
    return edit->pushToRootDB(*engine->DBROOT.get(), GStrToCStr(trackTitleToPush));
}

bool
EditorWrapper::pushToRootDB(String musicTitle, String musicComposer)
{
    if(edit == nullptr) return false;
    if(engine == nullptr) return false;
    if(!Flag_is_rendered) return false;
    return
    edit->pushToRootDB(*engine->DBROOT.get(),
        GStrToCStr(musicTitle),
        GStrToCStr(musicComposer));
}


bool
EditorWrapper::getMixDatas(Callable mixCallback)
{
    if(edit == nullptr) return false;
    if(!mixCallback.is_valid()) return false;
    edit->getAll<EDIT_ARG_MIX>(
        [&mixCallback](const EDIT_ARG_MIX& margs){
            mixCallback.call(
                static_cast<int>(margs.type),
                margs.ID,
                static_cast<int>(margs.details),
                CStrToGStr(margs.first),
                CStrToGStr(margs.second),
                CStrToGStr(margs.third),
                static_cast<int>(margs.bar),
                static_cast<int>(margs.beat),
                static_cast<int>(margs.separate),
                static_cast<int>(margs.Ebar),
                static_cast<int>(margs.Ebeat),
                static_cast<int>(margs.Eseparate)
            );
        }
    );
    return true;
}

bool
EditorWrapper::getMusicDatas(Callable musicCallback)
{
    if(edit == nullptr) return false;
    if(!musicCallback.is_valid()) return false;
    edit->getAll<EDIT_ARG_MUSIC>(
        [&musicCallback](const EDIT_ARG_MUSIC& margs){
            musicCallback.call(
                CStrToGStr( margs.musicName),
                static_cast<int>(margs.arg.bar),
                static_cast<int>(margs.arg.beat),
                CStrToGStr(margs.arg.bpm),
                static_cast<int>(margs.arg.separate)
            );
        }
    );
    return true;
}


bool
EditorWrapper::getNoteDatas(Callable noteCallback)
{
    if(edit == nullptr) return false;
    if(!noteCallback.is_valid()) return false;
    edit->getAll<EDIT_ARG_NOTE>(
        [&noteCallback](const EDIT_ARG_NOTE& margs){
            noteCallback.call(
                CStrToGStr(margs.Note_Type),
                CStrToGStr(margs.Note_Detail),
                CStrToGStr(margs.first),
                CStrToGStr(margs.second),
                CStrToGStr(margs.third),
                static_cast<int>(margs.bar),
                static_cast<int>(margs.beat),
                static_cast<int>(margs.separate),
                static_cast<int>(margs.Ebar),
                static_cast<int>(margs.Ebeat),
                static_cast<int>(margs.Eseparate)
            );
        }
    );
    return true;
}

bool
EditorWrapper::getKeyValueDatas(Callable KVCallback)
{
    if(edit == nullptr) return false;
    if(!KVCallback.is_valid()) return false;
    edit->getAll<EDIT_ARG_KEY_VALUE>(
        [&KVCallback](const EDIT_ARG_KEY_VALUE& margs){
            KVCallback.call(
                CStrToGStr(margs.first),
                CStrToGStr(margs.second)
            );
        }
    );
    return true;
}


Dictionary
EditorWrapper::getAll()
{
    if(edit == nullptr) return Dictionary();
    Dictionary result;
    Array mixs;
    
    edit->getAll<EDIT_ARG_MIX>(
        [&mixs](const EDIT_ARG_MIX& margs){
            Dictionary mixline;
            switch(margs.type){
                case TypeEnum::FILTER:
                    mixline["type"] = "FILTER";
                    break;
                case TypeEnum::EQ:
                    mixline["type"] = "EQ";
                    break;
                case TypeEnum::DISTORTION:
                    mixline["type"] = "DISTORTION";
                    break;
                case TypeEnum::CONTROL:
                    mixline["type"] = "CONTROL";
                    break;
                case TypeEnum::VOL:
                    mixline["type"] = "VOL";
                    break;
                case TypeEnum::LOAD:
                    mixline["type"] = "LOAD";
                    break;
                case TypeEnum::UNLOAD:
                    mixline["type"] = "UNLOAD";
                    break;
                case TypeEnum::BPM_CONTROL:
                    mixline["type"] = "BPM_CONTROL";
                    break;
                case TypeEnum::ECHO:
                    mixline["type"] = "ECHO";
                    break;
                case TypeEnum::OSC_FILTER:
                    mixline["type"] = "OSC_FILTER";
                    break;
                case TypeEnum::FLANGER:
                    mixline["type"] = "FLANGER";
                    break;
                case TypeEnum::PHASER:
                    mixline["type"] = "PHASER";
                    break;
                case TypeEnum::TRANCE:
                    mixline["type"] = "TRANCE";
                    break;
                case TypeEnum::PANNER:
                    mixline["type"] = "PANNER";
                    break;
                case TypeEnum::BATTLE_DJ:
                    mixline["type"] = "BATTLE_DJ";
                    break;
                case TypeEnum::ROLL:
                    mixline["type"] = "ROLL";
                    break;
                case TypeEnum::COMPRESSOR:
                    mixline["type"] = "COMPRESSOR";
                    break;
                case TypeEnum::ROBOT:
                    mixline["type"] = "ROBOT";
                    break;
                default:
                    mixline["type"] = "UNKNOWN";
                    break;
            };

            switch (margs.details)
            {

            case DetailEnum::HIGH:
                mixline["detail"] = "HIGH";
                break;
            case DetailEnum::MID:
                mixline["detail"] = "MID";
                break;
            case DetailEnum::LOW:
                mixline["detail"] = "LOW";
                break;
            case DetailEnum::PAUSE:
                mixline["detail"] = "PAUSE";
                break;
            case DetailEnum::CUE:
                mixline["detail"] = "CUE";
                break;
            case DetailEnum::TRIM:
                mixline["detail"] = "TRIM";
                break;
            case DetailEnum::FADER:
                mixline["detail"] = "FADER";
                break;
            case DetailEnum::TIME_STRETCH:
                mixline["detail"] = "TIME_STRETCH";
                break;
            case DetailEnum::SPIN:
                mixline["detail"] = "SPIN";
                break;
            case DetailEnum::PITCH:
                mixline["detail"] = "PITCH";
                break;
            case DetailEnum::REV:
                mixline["detail"] = "REV";
                break;
            case DetailEnum::SCRATCH:
                mixline["detail"] = "SCRATCH";
                break;
            case DetailEnum::BSCRATCH:
                mixline["detail"] = "BSCRATCH";
                break;
            default:
                mixline["detail"] = "UNKNOWN";
                break;
            }
            mixline ["ID"]          = margs.ID;
            mixline["first"]        = CStrToGStr(margs.first);
            mixline["second"]       = CStrToGStr(margs.second);
            mixline["third"]        = CStrToGStr(margs.third);
            mixline ["bar"]         = static_cast<int>(margs.bar);
            mixline ["beat"]        = static_cast<int>(margs.beat);
            mixline ["separate"]    = static_cast<int>(margs.separate);
            mixline ["Ebar"]        = static_cast<int>(margs.Ebar);
            mixline ["Ebeat"]       = static_cast<int>(margs.Ebeat);
            mixline ["Eseparate"]   = static_cast<int>(margs.Eseparate);
            mixs.push_back(mixline);
        }
    );
    result["mixDatas"] = mixs;
    Array musics;
    edit->getAll<EDIT_ARG_MUSIC>(
        [&musics](const EDIT_ARG_MUSIC& margs){
            Dictionary musicline;
            musicline["musicName"]  = CStrToGStr( margs.musicName);
            musicline["bar"]        = static_cast<int>(margs.arg.bar);
            musicline["beat"]       = static_cast<int>(margs.arg.beat);
            musicline["bpm"]        = CStrToGStr(margs.arg.bpm);
            musicline["separate"]   = static_cast<int>(margs.arg.separate);
            musics.push_back(musicline);
        }
    );
    result["musicDatas"] = musics;

    Array notes;
    edit->getAll<EDIT_ARG_NOTE>(
        [&notes](const EDIT_ARG_NOTE& margs){
            Dictionary noteline;
            
            noteline["Note_Type"]   = CStrToGStr(margs.Note_Type);
            noteline["Note_Detail"] = CStrToGStr(margs.Note_Detail);
            noteline["first"]       = CStrToGStr(margs.first);
            noteline["second"]      = CStrToGStr(margs.second);
            noteline["third"]       = CStrToGStr(margs.third);
            noteline["bar"]         = static_cast<int>(margs.bar);
            noteline["beat"]        = static_cast<int>(margs.beat);
            noteline["separate"]    = static_cast<int>(margs.separate);
            noteline["Ebar"]        = static_cast<int>(margs.Ebar);
            noteline["Ebeat"]       = static_cast<int>(margs.Ebeat);
            noteline["Eseparate"]   = static_cast<int>(margs.Eseparate);
            notes.push_back(noteline);
        }
    );
    result["noteDatas"] = notes;

    Dictionary keyValues;
    edit->getAll<EDIT_ARG_KEY_VALUE>(
        [&keyValues](const EDIT_ARG_KEY_VALUE& margs){
            keyValues[CStrToGStr( margs.first)] = CStrToGStr(margs.second);
        }
    );
    result["musicDatas"] = keyValues;
    return result;
}

