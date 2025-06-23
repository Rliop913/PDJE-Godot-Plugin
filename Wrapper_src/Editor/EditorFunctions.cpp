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
    ClassDB::bind_method(
		D_METHOD("AddLine", "arg"),
		&EditorWrapper::AddLine);
    ClassDB::bind_method(
		D_METHOD("EditMusicFirstBar", "title", "firstBar"),
		&EditorWrapper::EditMusicFirstBar);
    ClassDB::bind_method(
		D_METHOD("deleteLine", "obj", "skipType_if_mix_obj", "skipDetail_if_mix_obj"),
		&EditorWrapper::deleteLine);
    ClassDB::bind_method(
		D_METHOD("render", "trackTitle"),
		&EditorWrapper::render);
    ClassDB::bind_method(
		D_METHOD("demoPlayInit", "frameBufferSize", "trackTitle"),
		&EditorWrapper::demoPlayInit);
    ClassDB::bind_method(
		D_METHOD("pushTrackToRootDB", "trackTitleToPush"),
		&EditorWrapper::pushTrackToRootDB);
    ClassDB::bind_method(
		D_METHOD("pushToRootDB", "musicTitle", "musicComposer"),
		&EditorWrapper::pushToRootDB);
    ClassDB::bind_method(
		D_METHOD("getMixDatas", "mixCallback"),
		&EditorWrapper::getMixDatas);
    ClassDB::bind_method(
		D_METHOD("getMusicDatas", "musicCallback"),
		&EditorWrapper::getMusicDatas);
    ClassDB::bind_method(
		D_METHOD("getNoteDatas", "noteCallback"),
		&EditorWrapper::getNoteDatas);
    ClassDB::bind_method(
		D_METHOD("getKeyValueDatas", "KVCallback"),
		&EditorWrapper::getKeyValueDatas);
    ClassDB::bind_method(
		D_METHOD("getAll"),
		&EditorWrapper::getAll);
    ClassDB::bind_method(
		D_METHOD("Undo", "_FLAG_EDITOR_OBJ", "musicName_if_flag_music"),
		&EditorWrapper::Undo);
    ClassDB::bind_method(
		D_METHOD("Redo", "_FLAG_EDITOR_OBJ", "musicName_if_flag_music"),
		&EditorWrapper::Redo);
    ClassDB::bind_method(
		D_METHOD("Go", "_FLAG_EDITOR_OBJ", "branchName", "TimeNodeID"),
		&EditorWrapper::Go);
    ClassDB::bind_method(
		D_METHOD("GetLogWithJSONGraph", "_FLAG_EDITOR_OBJ", "musicName_if_flag_music"),
		&EditorWrapper::GetLogWithJSONGraph);
    ClassDB::bind_method(
		D_METHOD("UpdateLog", "_FLAG_EDITOR_OBJ", "musicName_if_flag_music"),
		&EditorWrapper::UpdateLog);
    ClassDB::bind_method(
		D_METHOD("DESTROY_PROJECT"),
		&EditorWrapper::DESTROY_PROJECT);
    ClassDB::bind_method(
		D_METHOD("Open", "projectPath"),
		&EditorWrapper::Open);
    ClassDB::bind_method(
		D_METHOD("ConfigNewMusic", "NewMusicName", "composer", "musicPath", "firstBar"),
		&EditorWrapper::ConfigNewMusic, DEFVAL("0"));
    ClassDB::bind_method(
		D_METHOD("GetDiff", "_FLAG_EDITOR_OBJ", "musicName_if_flag_music", "oldTimeNodeID", "newTimeNodeID"),
		&EditorWrapper::GetDiff);
    ClassDB::bind_method(
		D_METHOD("GetLastErr"),
		&EditorWrapper::GetLastErr);
}

String
EditorWrapper::GetLastErr()
{
    if(edit == nullptr) return "";
    return CStrToGStr(edit->RECENT_ERR);
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

constexpr int GIT_OID_BUFFER_SIZE = GIT_OID_HEXSZ + 1;

bool
EditorWrapper::Go(
    const int _FLAG_EDITOR_OBJ, 
    String branchName, 
    String NodeID)
{
    if(edit == nullptr) return false;
    git_oid commit_oid;
    int error = git_oid_fromstr(&commit_oid, GStrToCStr(NodeID).c_str());
    if (error != 0) {
        const git_error* err = git_error_last();
        godot::print_error("Failed to get nodeid: " + String(err->message));
        return false;
    }
    switch (_FLAG_EDITOR_OBJ)
    {
    case 0:
        return edit->Go<EDIT_ARG_NOTE>(GStrToCStr(branchName), &commit_oid);
    case 1:
        return edit->Go<EDIT_ARG_KEY_VALUE>(GStrToCStr(branchName), &commit_oid);
    case 2:
        return edit->Go<EDIT_ARG_MIX>(GStrToCStr(branchName), &commit_oid);
    case 3:
        return edit->Go<EDIT_ARG_MUSIC>(GStrToCStr(branchName), &commit_oid);
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
    
    return edit->Open(GpathToCPath(projectPath));
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
        GpathToCPath(musicPath),
        GStrToCStr(firstBar)
    );
}

Dictionary
DiffResultToDictionary(const DiffResult& dresult)
{
    auto godot_dictionary = Dictionary();
    auto newArray = Array();
    for(auto newlines : dresult.NewLines){
        auto newPair = Array();
        newPair.push_back(newlines.linenumber);
        newPair.push_back(CStrToGStr(newlines.text));
        newArray.push_back(newPair);
    }
    godot_dictionary["NewLines"] = newArray;
    auto oldArray = Array();
    for(auto oldlines : dresult.OldLines){
        auto oldPair = Array();
        oldPair.push_back(oldlines.linenumber);
        oldPair.push_back(CStrToGStr(oldlines.text));
        oldArray.push_back(oldPair);
    }
    godot_dictionary["OldLines"] = oldArray;
    return std::move(godot_dictionary);
}



Dictionary
EditorWrapper::GetDiff(
    const int _FLAG_EDITOR_OBJ,
    String musicName_if_flag_music,
    String oldNodeID, 
    String newNodeID)
{
    if(edit == nullptr) return Dictionary();
    git_oid old_oid;
    git_oid new_oid;
    git_repository* baseRepo = nullptr;

    int olderror = git_oid_fromstr(&old_oid, GStrToCStr(oldNodeID).c_str());
    int newerror = git_oid_fromstr(&new_oid, GStrToCStr(newNodeID).c_str());
    

    if (olderror != 0 || newerror != 0) {
        const git_error* err = git_error_last();
        godot::print_error("Failed to get nodeid: " + String(err->message));
        return Dictionary();
    }


    auto CommitSetter = [&old_oid, &new_oid](std::optional<gitwrap::commit>& oldC, std::optional<gitwrap::commit>& newC, git_repository*& basePTR){
        if(basePTR == nullptr){
            return;
        }
        oldC.emplace(gitwrap::commit(old_oid, basePTR));
        newC.emplace(gitwrap::commit(new_oid, basePTR));
        return;
    };
    std::optional<gitwrap::commit> newC;
    std::optional<gitwrap::commit> oldC;
    std::optional<DiffResult> Dresult;
    switch (_FLAG_EDITOR_OBJ)
    {
    case 0:
        baseRepo = edit->getNoteRepo();
        CommitSetter(oldC, newC, baseRepo);
        if(oldC.has_value() && newC.has_value()){
            Dresult = edit->GetDiff<EDIT_ARG_NOTE>(oldC.value(), newC.value());
        }
        break;
    case 1:
        baseRepo = edit->getKVRepo();
        CommitSetter(oldC, newC, baseRepo);
        if(oldC.has_value() && newC.has_value()){
            Dresult = edit->GetDiff<EDIT_ARG_KEY_VALUE>(oldC.value(), newC.value());
        }
        break;
    case 2:
        baseRepo = edit->getMixRepo();
        CommitSetter(oldC, newC, baseRepo);
        if(oldC.has_value() && newC.has_value()){
            Dresult = edit->GetDiff<EDIT_ARG_MIX>(oldC.value(), newC.value());
        }
        break;
    case 3:
        baseRepo = edit->getMusicRepo(GStrToCStr(musicName_if_flag_music));
        CommitSetter(oldC, newC, baseRepo);
        if(oldC.has_value() && newC.has_value()){
            Dresult = edit->GetDiff<EDIT_ARG_MUSIC>(oldC.value(), newC.value());
        }
        break;
    default:
        return Dictionary();
    }

    if(Dresult.has_value()) return DiffResultToDictionary(Dresult.value());
    else return Dictionary();
}

