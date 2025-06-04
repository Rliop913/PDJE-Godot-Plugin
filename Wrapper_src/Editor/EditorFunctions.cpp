#include "EditorWrapper.hpp"
#include "godot_cpp/classes/project_settings.hpp"
using namespace godot;

VARIANT_ENUM_CAST(EditorWrapper::FLAG_EDITOR_OBJ);
void
EditorWrapper::_bind_methods()
{
    BIND_ENUM_CONSTANT(NOTE);
    BIND_ENUM_CONSTANT(KV);
    BIND_ENUM_CONSTANT(MIX);
    BIND_ENUM_CONSTANT(MUSIC);

}


bool
EditorWrapper::Undo(const int _FLAG_EDITOR_OBJ , String musicName_if_flag_music)
{
    if(edit == nullptr) return false;
    switch (_FLAG_EDITOR_OBJ)
    {
    case 0:
        return edit->Undo<EDIT_ARG_NOTE>();
    case 1:
        return edit->Undo<EDIT_ARG_KEY_VALUE>();
    case 2:
        return edit->Undo<EDIT_ARG_MIX>();
    case 3:
        return edit->Undo<EDIT_ARG_MUSIC>(GStrToCStr(musicName_if_flag_music));
    default:
        return false;
    }
}

bool
EditorWrapper::Redo(const int _FLAG_EDITOR_OBJ , String musicName_if_flag_music)
{
    if(edit == nullptr) return false;
    switch (_FLAG_EDITOR_OBJ)
    {
    case 0:
        return edit->Redo<EDIT_ARG_NOTE>();
    case 1:
        return edit->Redo<EDIT_ARG_KEY_VALUE>();
    case 2:
        return edit->Redo<EDIT_ARG_MIX>();
    case 3:
        return edit->Redo<EDIT_ARG_MUSIC>(GStrToCStr(musicName_if_flag_music));
    default:
        return false;
    }
}

bool
EditorWrapper::Go(
    const int _FLAG_EDITOR_OBJ, 
    String branchName, 
    git_oid* commitID)
{
    if(edit == nullptr) return false;
    switch (_FLAG_EDITOR_OBJ)
    {
    case 0:
        return edit->Go<EDIT_ARG_NOTE>(GStrToCStr(branchName), commitID);
    case 1:
        return edit->Go<EDIT_ARG_KEY_VALUE>(GStrToCStr(branchName), commitID);
    case 2:
        return edit->Go<EDIT_ARG_MIX>(GStrToCStr(branchName), commitID);
    case 3:
        return edit->Go<EDIT_ARG_MUSIC>(GStrToCStr(branchName), commitID);
    default:
        return false;
    }
}

String
EditorWrapper::GetLogWithJSONGraph(const int _FLAG_EDITOR_OBJ, String musicName_if_flag_music)
{
    if(edit == nullptr) return "edit nullptr";
    switch (_FLAG_EDITOR_OBJ)
    {
    case 0:
        return CStrToGStr(edit->GetLogWithJSONGraph<EDIT_ARG_NOTE>());
    case 1:
        return CStrToGStr(edit->GetLogWithJSONGraph<EDIT_ARG_KEY_VALUE>());
    case 2:
        return CStrToGStr(edit->GetLogWithJSONGraph<EDIT_ARG_MIX>());
    case 3:
        return CStrToGStr(edit->GetLogWithJSONGraph<EDIT_ARG_MUSIC>(GStrToCStr(musicName_if_flag_music)));
    default:
        return "no match flag";
    }
}

bool
EditorWrapper::UpdateLog(const int _FLAG_EDITOR_OBJ, String branchName)
{
    if(edit == nullptr) return false;
    switch (_FLAG_EDITOR_OBJ)
    {
    case 0:
        if(branchName.is_empty()){
            return edit->UpdateLog<EDIT_ARG_NOTE>();
        }
        else{
            return edit->UpdateLog<EDIT_ARG_NOTE>(GStrToCStr(branchName));
        }
    case 1:
        if(branchName.is_empty()){
            return edit->UpdateLog<EDIT_ARG_KEY_VALUE>();
        }
        else{
            return edit->UpdateLog<EDIT_ARG_KEY_VALUE>(GStrToCStr(branchName));
        }
    case 2:
        if(branchName.is_empty()){
            return edit->UpdateLog<EDIT_ARG_MIX>();
        }
        else{
            return edit->UpdateLog<EDIT_ARG_MIX>(GStrToCStr(branchName));
        }
    case 3:
        if(branchName.is_empty()){
            return edit->UpdateLog<EDIT_ARG_MUSIC>();
        }
        else{
            return edit->UpdateLog<EDIT_ARG_MUSIC>(GStrToCStr(branchName));
        }
    default:
        return false;
    }
}

String
EditorWrapper::DESTROY_PROJECT()
{
    if(edit == nullptr) return "edit nullptr";
    return CStrToGStr(edit->DESTROY_PROJECT());
}

bool
EditorWrapper::Open(String projectPath)
{
    if(edit == nullptr) return false;
    auto gpath = ProjectSettings::get_singleton()->globalize_path(projectPath);
    return edit->Open(GStrToCStr(gpath));
}

bool
EditorWrapper::ConfigNewMusic(
    String NewMusicName, 
    String composer,
    String musicPath,
    String firstBar)
{
    if(edit == nullptr) return false;
    return 
    edit->ConfigNewMusic(
        GStrToCStr(NewMusicName),
        GStrToCStr(composer),
        GStrToCStr(musicPath),
        GStrToCStr(firstBar)
    );
}


DiffResult
EditorWrapper::GetDiff(
    const int _FLAG_EDITOR_OBJ, 
    const gitwrap::commit& oldTimeStamp, 
    const gitwrap::commit& newTimeStamp)
{
    if(edit == nullptr) return DiffResult();
    switch (_FLAG_EDITOR_OBJ)
    {
    case 0:
        return edit->GetDiff<EDIT_ARG_NOTE>(oldTimeStamp, newTimeStamp);
    case 1:
        return edit->GetDiff<EDIT_ARG_KEY_VALUE>(oldTimeStamp, newTimeStamp);
    case 2:
        return edit->GetDiff<EDIT_ARG_MIX>(oldTimeStamp, newTimeStamp);
    case 3:
        return edit->GetDiff<EDIT_ARG_MUSIC>(oldTimeStamp, newTimeStamp);
    default:
        return DiffResult();
    }
}