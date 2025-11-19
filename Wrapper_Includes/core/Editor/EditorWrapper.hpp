#pragma once

#include <godot_cpp/classes/ref.hpp>

#include "PDJE_interface.hpp"
#include "WrappedEditorArgs.hpp"
namespace godot {

class EditorWrapper : public RefCounted {
    GDCLASS(EditorWrapper, RefCounted)

  private:
    editorObject *edit;
    PDJE         *engine;
    bool          Flag_is_rendered = false;

  protected:
    static void
    _bind_methods();

  public:
    enum FLAG_EDITOR_OBJ { NOTE = 0, KV, MIX, MUSIC };
    bool
    AddLine(Ref<PDJE_EDITOR_ARG> arg);
    bool
    EditMusicFirstBeat(String title, String firstBeat);
    int
    deleteLine(Ref<PDJE_EDITOR_ARG> obj,
               bool                 skipType_if_mix_obj,
               bool                 skipDetail_if_mix_obj);

    String
    render(String trackTitle);

    bool
    demoPlayInit(unsigned int frameBufferSize, String trackTitle);

    bool
    pushTrackToRootDB(String trackTitleToPush);

    bool
    pushToRootDB(String musicTitle, String musicComposer);

    bool
    getMixDatas();
    bool
    getMusicBpmDatas();
    bool
    getNoteDatas();
    bool
    getKeyValueDatas();
    Dictionary
    getAll();

    String
    Undo(const int _FLAG_EDITOR_OBJ, String musicName_if_flag_music = "");

    String
    Redo(const int _FLAG_EDITOR_OBJ, String musicName_if_flag_music = "");

    String
    Go(const int _FLAG_EDITOR_OBJ, String OID);

    String
    GetLogWithJSONGraph(const int _FLAG_EDITOR_OBJ, String musicName);

    String
    UpdateLog();

    /// WARNING!!! THERE IS NO TURNING BACK
    String
    DESTROY_PROJECT();

    void
    Init(editorObject *refobj, PDJE *refengine);

    bool
    ConfigNewMusic(String NewMusicName,
                   String composer,
                   String musicPath,
                   String firstBar = "0");

    Dictionary
    GetDiff(const int _FLAG_EDITOR_OBJ,
            String    musicName_if_flag_music,
            String    from_OID,
            String    to_OID);
    EditorWrapper()  = default;
    ~EditorWrapper() = default;
};
} // namespace godot