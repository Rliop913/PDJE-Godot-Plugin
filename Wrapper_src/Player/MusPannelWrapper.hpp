#pragma once

#include <godot_cpp/classes/ref.hpp>

#include "PDJE_interface.hpp"

#include "FXWrapper.hpp"

namespace godot {

class MusPannelWrapper : public RefCounted {
    GDCLASS(MusPannelWrapper, RefCounted)
  private:
    MusicControlPannel *musref = nullptr;
    PDJE               *engine = nullptr;

  protected:
    static void
    _bind_methods();

  public:
    void
    Init(MusicControlPannel *refobj, PDJE *refengine);

    int
    LoadMusic(String Title, String composer, double bpm = -1.0);

    bool
    CueMusic(String title, String newPos);
    bool
    SetMusic(String title, const bool onOff);
    Array
    GetLoadedMusicList();
    bool
    UnloadMusic(String title);
    Ref<FXWrapper>
    getFXHandle(String title);
    MusPannelWrapper()  = default;
    ~MusPannelWrapper() = default;
};
} // namespace godot