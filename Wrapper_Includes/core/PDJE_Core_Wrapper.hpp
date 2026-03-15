#pragma once
#include "CoreLine.hpp"
#include "EditorWrapper.hpp"
#include "MusPanelWrapper.hpp"
#include "PDJE_interface.hpp"
#include "PlayerWrapper.hpp"
#include "pdje_util_common.hpp"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
// #include "PDJE_interface.hpp"

namespace godot {

class PDJE_Wrapper : public Node {
    GDCLASS(PDJE_Wrapper, Node)

  private:
  protected:
    static void
    _bind_methods();

  public:
    std::optional<PDJE> engine;
    enum PDJE_PLAY_MODE {
        FULL_PRE_RENDER = 0,
        HYBRID_RENDER,
        FULL_MANUAL_RENDER
    };
    bool
    InitEngine(String DBPath);
    bool
    InitEditor(String authName, String authEmail, String projectRoot);
    Array
    SearchMusic(String Title, String composer, double bpm = -1.0);
    Dictionary
    GetPCMFromMusicData(Dictionary musicData);
    Array
    SearchTrack(String Title);
    bool
    InitPlayer(PDJE_PLAY_MODE mode,
               String         trackTitle,
               unsigned int   FrameBufferSize);
    void
    ResetPlayer();

    void
    CloseEditor();

    Ref<CoreLine>
    PullOutCoreLine();

    PDJE_CORE_DATA_LINE
    PullOutRawCoreLine();

    bool
    GetNoteObjects(String trackTitle);

    Ref<PlayerWrapper>
    GetPlayer();

    Ref<EditorWrapper>
    GetEditor();

    PDJE_Wrapper();
    ~PDJE_Wrapper();

    void
    _ready() override;
};

} // namespace godot
