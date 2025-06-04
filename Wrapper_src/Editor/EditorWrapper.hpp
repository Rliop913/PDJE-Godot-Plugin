#pragma once

#include <godot_cpp/classes/ref.hpp>
#include "PDJE_interface.hpp"
#include "EditorArgs.hpp"
namespace godot{


class EditorWrapper : public RefCounted{
    GDCLASS(EditorWrapper, RefCounted)

private:
    editorObject* edit;
    PDJE* engine;
protected:
    static void _bind_methods();
public:
    enum FLAG_EDITOR_OBJ{
        NOTE = 0,
        KV,
        MIX,
        MUSIC
    };
    bool AddLine(PDJE_EDITOR_ARG arg);
    bool EditMusicFirstBar(String title, String firstBar);
    int deleteLine(
        PDJE_EDITOR_ARG obj,
        bool skipType_if_mix_obj, 
        bool skipDetail_if_mix_obj);

    bool render(String trackTitle);

    bool demoPlayInit(unsigned int frameBufferSize, String trackTitle);

    bool pushTrackToRootDB(String& trackTitleToPush);

    bool pushToRootDB(String musicTitle, String musicComposer);

    Dictionary getAll();

    bool Undo(const int _FLAG_EDITOR_OBJ , String musicName_if_flag_music = "");

    bool Redo(const int _FLAG_EDITOR_OBJ , String musicName_if_flag_music = "");
    
    bool Go(const int _FLAG_EDITOR_OBJ, String branchName, git_oid* commitID);
    
    String GetLogWithJSONGraph(const int _FLAG_EDITOR_OBJ, String musicName);

    bool UpdateLog(const int _FLAG_EDITOR_OBJ, String branchName="");

    ///WARNING!!! THERE IS NO TURNING BACK
    String DESTROY_PROJECT();
    bool Open(String projectPath);

    void Init(editorObject* refobj, PDJE* refengine);

    bool ConfigNewMusic(String NewMusicName, 
                        String composer,
                        String musicPath,
                        String firstBar = "0");
    DiffResult GetDiff(const int _FLAG_EDITOR_OBJ, const gitwrap::commit& oldTimeStamp, const gitwrap::commit& newTimeStamp);
//todo-implement below





// nj& operator[](const std::string& key){
//     return E_obj->KVHandler.second[key];
// }





    EditorWrapper() = default;
    ~EditorWrapper() = default;
};
}