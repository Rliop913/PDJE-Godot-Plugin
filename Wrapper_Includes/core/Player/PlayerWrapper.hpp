#pragma once

#include <godot_cpp/classes/ref.hpp>

#include "FXWrapper.hpp"
#include "MusPanelWrapper.hpp"
#include "PDJE_interface.hpp"

namespace godot {

class PlayerWrapper : public RefCounted {
    GDCLASS(PlayerWrapper, RefCounted)
  private:
    audioPlayer *playerobj = nullptr;
    PDJE        *engineobj = nullptr;

  protected:
    static void
    _bind_methods();

  public:
    bool
    Activate();
    bool
    Deactivate();
    Ref<FXWrapper>
    GetFXControlPanel();
    Ref<MusPanelWrapper>
    GetMusicControlPanel();
    bool
    ChangeCursorPos(String pos);
    String
    GetConsumedFrames();
    String
    GetStatus();
    void
    Init(audioPlayer *refobj, PDJE *refengine);
    PlayerWrapper()  = default;
    ~PlayerWrapper() = default;
};
} // namespace godot