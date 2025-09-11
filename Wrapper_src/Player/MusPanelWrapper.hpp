#pragma once

#include <godot_cpp/classes/ref.hpp>

#include "PDJE_interface.hpp"

#include "FXWrapper.hpp"

namespace godot {

class MusPanelWrapper : public RefCounted {
    GDCLASS(MusPanelWrapper, RefCounted)
  private:
    MusicControlPanel *musref = nullptr;
    PDJE               *engine = nullptr;

  protected:
    static void
    _bind_methods();

  public:
    void
    Init(MusicControlPanel *refobj, PDJE *refengine);

    int
    LoadMusic(String Title, String composer, double bpm = -1.0);

    bool
    CueMusic(String title, String newPos);
    bool
    SetMusic(String title, const bool onOff);
    bool
    ChangeBpm(String title, double targetBpm, double originBpm);
    Array
    GetLoadedMusicList();
    bool
    UnloadMusic(String title);
    Ref<FXWrapper>
    getFXHandle(String title);

    MusPanelWrapper()  = default;
    ~MusPanelWrapper() = default;
};
} // namespace godot