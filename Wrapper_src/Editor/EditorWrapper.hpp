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


//todo-implement below
template<typename EDIT_ARG_TYPE> 
bool Undo();

template<typename EDIT_ARG_TYPE> 
bool Undo(const std::string& musicName);


template<typename EDIT_ARG_TYPE> 
bool Redo();

template<typename EDIT_ARG_TYPE> 
bool Redo(const std::string& musicName);

template<typename EDIT_ARG_TYPE> 
bool Go(const std::string& branchName, git_oid* commitID);

template<typename EDIT_ARG_TYPE> 
std::string GetLogWithJSONGraph();

template<typename EDIT_ARG_TYPE> 
std::string GetLogWithJSONGraph(const std::string& musicName);


template<typename EDIT_ARG_TYPE> 
bool UpdateLog();

template<typename EDIT_ARG_TYPE> 
bool UpdateLog(const std::string& branchName);

template<typename EDIT_ARG_TYPE> 
DiffResult GetDiff(const gitwrap::commit& oldTimeStamp, const gitwrap::commit& newTimeStamp);

nj& operator[](const std::string& key){
    return E_obj->KVHandler.second[key];
}

///WARNING!!! THERE IS NO TURNING BACK
std::string DESTROY_PROJECT();

bool ConfigNewMusic(const std::string& NewMusicName, 
                    const std::string& composer,
                    const std::string& musicPath,
                    const std::string& firstBar = "0");


bool Open(const std::string& projectPath);

    void Init(editorObject* refobj, PDJE* refengine);
    EditorWrapper() = default;
    ~EditorWrapper() = default;
};
}