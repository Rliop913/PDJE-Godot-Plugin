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
    ClassDB::bind_method(D_METHOD("AddLine", "arg"), &EditorWrapper::AddLine);
    ClassDB::bind_method(D_METHOD("EditMusicFirstBar", "title", "firstBar"),
                         &EditorWrapper::EditMusicFirstBeat);
    ClassDB::bind_method(D_METHOD("deleteLine",
                                  "obj",
                                  "skipType_if_mix_obj",
                                  "skipDetail_if_mix_obj"),
                         &EditorWrapper::deleteLine);
    ClassDB::bind_method(D_METHOD("render", "trackTitle"),
                         &EditorWrapper::render);
    ClassDB::bind_method(
        D_METHOD("demoPlayInit", "frameBufferSize", "trackTitle"),
        &EditorWrapper::demoPlayInit);
    ClassDB::bind_method(D_METHOD("pushTrackToRootDB", "trackTitleToPush"),
                         &EditorWrapper::pushTrackToRootDB);
    ClassDB::bind_method(
        D_METHOD("pushToRootDB", "musicTitle", "musicComposer"),
        &EditorWrapper::pushToRootDB);
    ClassDB::bind_method(D_METHOD("getMixDatas", "mixCallback"),
                         &EditorWrapper::getMixDatas);
    ClassDB::bind_method(D_METHOD("getMusicDatas", "musicCallback"),
                         &EditorWrapper::getMusicDatas);
    ClassDB::bind_method(D_METHOD("getNoteDatas", "noteCallback"),
                         &EditorWrapper::getNoteDatas);
    ClassDB::bind_method(D_METHOD("getKeyValueDatas", "KVCallback"),
                         &EditorWrapper::getKeyValueDatas);
    ClassDB::bind_method(D_METHOD("getAll"), &EditorWrapper::getAll);
    ClassDB::bind_method(
        D_METHOD("Undo", "_FLAG_EDITOR_OBJ", "musicName_if_flag_music"),
        &EditorWrapper::Undo);
    ClassDB::bind_method(
        D_METHOD("Redo", "_FLAG_EDITOR_OBJ", "musicName_if_flag_music"),
        &EditorWrapper::Redo);
    ClassDB::bind_method(
        D_METHOD("Go", "_FLAG_EDITOR_OBJ", "branchName", "TimeNodeID"),
        &EditorWrapper::Go);
    ClassDB::bind_method(D_METHOD("GetLogWithJSONGraph",
                                  "_FLAG_EDITOR_OBJ",
                                  "musicName_if_flag_music"),
                         &EditorWrapper::GetLogWithJSONGraph);
    ClassDB::bind_method(
        D_METHOD("UpdateLog", "_FLAG_EDITOR_OBJ", "musicName_if_flag_music"),
        &EditorWrapper::UpdateLog);
    ClassDB::bind_method(D_METHOD("DESTROY_PROJECT"),
                         &EditorWrapper::DESTROY_PROJECT);
    ClassDB::bind_method(D_METHOD("Open", "projectPath"), &EditorWrapper::Open);
    ClassDB::bind_method(D_METHOD("ConfigNewMusic",
                                  "NewMusicName",
                                  "composer",
                                  "musicPath",
                                  "firstBar"),
                         &EditorWrapper::ConfigNewMusic,
                         DEFVAL("0"));
    ClassDB::bind_method(D_METHOD("GetDiff",
                                  "_FLAG_EDITOR_OBJ",
                                  "musicName_if_flag_music",
                                  "oldTimeNodeID",
                                  "newTimeNodeID"),
                         &EditorWrapper::GetDiff);
}

String
EditorWrapper::Undo(const int _FLAG_EDITOR_OBJ, String musicName_if_flag_music)
{
    if (edit == nullptr)
        return "editor is null";
    switch (_FLAG_EDITOR_OBJ) {
    case 0:
        if(edit->Undo<EDIT_ARG_NOTE>()){
            return "NOTE Undo OK";
        }
        else{
            return "NOTE Undo Failed";
        }
    case 1:
        if(edit->Undo<EDIT_ARG_KEY_VALUE>()){
            return "KEY_VALUE Undo OK";
        }
        else{
            return "KEY_VALUE Undo Failed";
        }
    case 2:
        if(edit->Undo<EDIT_ARG_MIX>()){
            return "MIX Undo OK";
        }
        else{
            return "MIX Undo Failed";
        }
    case 3:
        if(edit->Undo<EDIT_ARG_MUSIC>(GStrToCStr(musicName_if_flag_music))){
            return "MUSIC Undo OK";
        }
        else{
            return "MUSIC Undo Failed";
        }
    default:
        return "arg didn't matched";
    }
}

String
EditorWrapper::Redo(const int _FLAG_EDITOR_OBJ, String musicName_if_flag_music)
{
    if (edit == nullptr)
        return "editor is null";
    switch (_FLAG_EDITOR_OBJ) {
    case 0:
        if(edit->Redo<EDIT_ARG_NOTE>()){
            return "NOTE Redo OK";
        }
        else{
            return "NOTE Redo Failed";
        }
    case 1:
        if(edit->Redo<EDIT_ARG_KEY_VALUE>()){
            return "KEY_VALUE Redo OK";
        }
        else{
            return "KEY_VALUE Redo Failed";
        }
    case 2:
        if(edit->Redo<EDIT_ARG_MIX>()){
            return "MIX Redo OK";
        }
        else{
            return "MIX Redo Failed";
        }
    case 3:
        if(edit->Redo<EDIT_ARG_MUSIC>(GStrToCStr(musicName_if_flag_music))){
            return "MUSIC Redo OK";
        }
        else{
            return "MUSIC Redo Failed";
        }
    default:
        return "arg didn't matched";
    }
}

constexpr int GIT_OID_BUFFER_SIZE = GIT_OID_HEXSZ + 1;

String
EditorWrapper::Go(const int _FLAG_EDITOR_OBJ, String branchName, String NodeID)
{
    if (edit == nullptr)
        return "editor is null";

    auto node_id = GStrToCStr(NodeID);
    switch (_FLAG_EDITOR_OBJ) {
    case 0:
        if(edit->Go<EDIT_ARG_NOTE>(GStrToCStr(branchName), node_id)){
            return "NOTE Go OK";
        }
        else{
            return "NOTE Go Failed";
        }
    case 1:
        if(edit->Go<EDIT_ARG_KEY_VALUE>(GStrToCStr(branchName), node_id)){
            return "KEY_VALUE Go OK";
        }
        else{
            return "KEY_VALUE Go Failed";
        }
    case 2:
        if(edit->Go<EDIT_ARG_MIX>(GStrToCStr(branchName), node_id)){
            return "MIX Go OK";
        }
        else{
            return "MIX Go Failed";
        }
    case 3:
        if(edit->Go<EDIT_ARG_MUSIC>(GStrToCStr(branchName), node_id)){
            return "MUSIC Go OK";
        }
        else{
            return "MUSIC Go Failed";
        }
    default:
        return "arg didn't matched";
    }
}

String
EditorWrapper::GetLogWithJSONGraph(const int _FLAG_EDITOR_OBJ,
                                   String    musicName_if_flag_music)
{
    if (edit == nullptr)
        return "editor is null";
    switch (_FLAG_EDITOR_OBJ) {
    case 0:
        return CStrToGStr(edit->GetLogWithJSONGraph<EDIT_ARG_NOTE>());
    case 1:
        return CStrToGStr(edit->GetLogWithJSONGraph<EDIT_ARG_KEY_VALUE>());
    case 2:
        return CStrToGStr(edit->GetLogWithJSONGraph<EDIT_ARG_MIX>());
    case 3:
        return CStrToGStr(edit->GetLogWithJSONGraph<EDIT_ARG_MUSIC>(
            GStrToCStr(musicName_if_flag_music)));
    default:
        return "no match flag";
    }
}

String
EditorWrapper::UpdateLog(const int _FLAG_EDITOR_OBJ, String branchName)
{
    if (edit == nullptr)
        return "editor is null";
    switch (_FLAG_EDITOR_OBJ) {
    case 0:
        if (branchName.is_empty()) {
            if(edit->UpdateLog<EDIT_ARG_NOTE>()){
                return "NOTE UpdateLog OK";
            }
            else{
                return "Note UpdateLog Failed";
            }
        } else {
            if(edit->UpdateLog<EDIT_ARG_NOTE>(GStrToCStr(branchName))){
                return "NOTE branch UpdateLog OK";
            }
            else{
                return "NOTE branch UpdateLog Failed";
            }
        }
    case 1:
        if (branchName.is_empty()) {
            if(edit->UpdateLog<EDIT_ARG_KEY_VALUE>()){
                return "KV UpdateLog OK";
            }
            else{
                return "KV UpdateLog Failed";
            }
        } else {
            if(edit->UpdateLog<EDIT_ARG_KEY_VALUE>(GStrToCStr(branchName))){
                return "KV branch UpdateLog OK";
            }
            else{
                return "KV branch UpdateLog Failed";
            }
        }
    case 2:
        if (branchName.is_empty()) {
            if(edit->UpdateLog<EDIT_ARG_MIX>()){
                return "MIX UpdateLog OK";
            }
            else{
                return "MIX UpdateLog Failed";
            }
        } else {
            if(edit->UpdateLog<EDIT_ARG_MIX>(GStrToCStr(branchName))){
                return "MIX branch UpdateLog OK";
            }
            else{
                return "MIX branch UpdateLog Failed";
            }
        }
    case 3:
        if (branchName.is_empty()) {
            if(edit->UpdateLog<EDIT_ARG_MUSIC>()){
                return "MUSIC UpdateLog OK";
            }
            else{
                return "MUSIC UpdateLog Failed";
            }
        } else {
            if(edit->UpdateLog<EDIT_ARG_MUSIC>(GStrToCStr(branchName))){
                return "MUSIC branch UpdateLog OK";
            }
            else{
                return "MUSIC branch UpdateLog Failed";
            }
        }
    default:
        return "no match flag";
    }
}

String
EditorWrapper::DESTROY_PROJECT()
{
    if (edit == nullptr)
        return "editor is null";
    return CStrToGStr(edit->DESTROY_PROJECT());
}

String
EditorWrapper::Open(String projectPath)
{
    if (edit == nullptr)
        return "editor is null";

    if(edit->Open(GpathToCPath(projectPath))){
        return "OK";
    }
    else{
        return "Open Failed";
    }
}

bool
EditorWrapper::ConfigNewMusic(String NewMusicName,
                              String composer,
                              String musicPath,
                              String firstBar)
{
    if (edit == nullptr)
        return false;
    return edit->ConfigNewMusic(GStrToCStr(NewMusicName),
                                GStrToCStr(composer),
                                GpathToCPath(musicPath),
                                GStrToCStr(firstBar));
}

Dictionary
DiffResultToDictionary(const DiffResult &dresult)
{
    auto godot_dictionary = Dictionary();
    auto newArray         = Array();
    for (auto newlines : dresult.NewLines) {
        auto newPair = Array();
        newPair.push_back(newlines.linenumber);
        newPair.push_back(CStrToGStr(newlines.text));
        newArray.push_back(newPair);
    }
    godot_dictionary["NewLines"] = newArray;
    auto oldArray                = Array();
    for (auto oldlines : dresult.OldLines) {
        auto oldPair = Array();
        oldPair.push_back(oldlines.linenumber);
        oldPair.push_back(CStrToGStr(oldlines.text));
        oldArray.push_back(oldPair);
    }
    godot_dictionary["OldLines"] = oldArray;
    return std::move(godot_dictionary);
}

Dictionary
EditorWrapper::GetDiff(const int _FLAG_EDITOR_OBJ,
                       String    musicName_if_flag_music,
                       String    oldNodeID,
                       String    newNodeID)
{
    if (edit == nullptr)
        return Dictionary();
    git_oid         old_oid;
    git_oid         new_oid;
    git_repository *baseRepo = nullptr;

    int olderror = git_oid_fromstr(&old_oid, GStrToCStr(oldNodeID).c_str());
    int newerror = git_oid_fromstr(&new_oid, GStrToCStr(newNodeID).c_str());

    if (olderror != 0 || newerror != 0) {
        const git_error *err = git_error_last();
        godot::print_error("Failed to get nodeid: " + String(err->message));
        return Dictionary();
    }

    auto CommitSetter = [&old_oid,
                         &new_oid](std::optional<gitwrap::commit> &oldC,
                                   std::optional<gitwrap::commit> &newC,
                                   git_repository                *&basePTR) {
        if (basePTR == nullptr) {
            return;
        }
        oldC.emplace(gitwrap::commit(old_oid, basePTR));
        newC.emplace(gitwrap::commit(new_oid, basePTR));
        return;
    };
    std::optional<gitwrap::commit> newC;
    std::optional<gitwrap::commit> oldC;
    std::optional<DiffResult>      Dresult;
    switch (_FLAG_EDITOR_OBJ) {
    case 0:
        baseRepo = edit->getNoteRepo();
        CommitSetter(oldC, newC, baseRepo);
        if (oldC.has_value() && newC.has_value()) {
            Dresult = edit->GetDiff<EDIT_ARG_NOTE>(oldC.value(), newC.value());
        }
        break;
    case 1:
        baseRepo = edit->getKVRepo();
        CommitSetter(oldC, newC, baseRepo);
        if (oldC.has_value() && newC.has_value()) {
            Dresult =
                edit->GetDiff<EDIT_ARG_KEY_VALUE>(oldC.value(), newC.value());
        }
        break;
    case 2:
        baseRepo = edit->getMixRepo();
        CommitSetter(oldC, newC, baseRepo);
        if (oldC.has_value() && newC.has_value()) {
            Dresult = edit->GetDiff<EDIT_ARG_MIX>(oldC.value(), newC.value());
        }
        break;
    case 3:
        baseRepo = edit->getMusicRepo(GStrToCStr(musicName_if_flag_music));
        CommitSetter(oldC, newC, baseRepo);
        if (oldC.has_value() && newC.has_value()) {
            Dresult = edit->GetDiff<EDIT_ARG_MUSIC>(oldC.value(), newC.value());
        }
        break;
    default:
        return Dictionary();
    }

    if (Dresult.has_value())
        return DiffResultToDictionary(Dresult.value());
    else
        return Dictionary();
}
