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
    bool Flag_is_rendered = false;
protected:
    static void _bind_methods();
public:
    enum FLAG_EDITOR_OBJ{
        NOTE = 0,
        KV,
        MIX,
        MUSIC
    };
    bool AddLine(Ref<PDJE_EDITOR_ARG> arg);
    bool EditMusicFirstBar(String title, String firstBar);
    int deleteLine(
        Ref<PDJE_EDITOR_ARG> obj,
        bool skipType_if_mix_obj, 
        bool skipDetail_if_mix_obj);

    bool render(String trackTitle);

    bool demoPlayInit(unsigned int frameBufferSize, String trackTitle);

    bool pushTrackToRootDB(String trackTitleToPush);

    bool pushToRootDB(String musicTitle, String musicComposer);

    bool getMixDatas(Callable mixCallback);
    bool getMusicDatas(Callable musicCallback);
    bool getNoteDatas(Callable noteCallback);
    bool getKeyValueDatas(Callable KVCallback);
    Dictionary getAll();

    bool Undo(const int _FLAG_EDITOR_OBJ , String musicName_if_flag_music = "");

    bool Redo(const int _FLAG_EDITOR_OBJ , String musicName_if_flag_music = "");
    
    bool Go(const int _FLAG_EDITOR_OBJ, String branchName, String TimeNodeID);
    
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
                        
    Dictionary GetDiff(
        const int _FLAG_EDITOR_OBJ, 
        String musicName_if_flag_music, 
        String oldTimeNodeID, 
        String newTimeNodeID);
    EditorWrapper() = default;
    ~EditorWrapper() = default;
};
}