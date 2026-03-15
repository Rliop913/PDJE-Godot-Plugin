#include "PlayerWrapper.hpp"
#include "FXWrapper.hpp"
#include "MusPanelWrapper.hpp"
#include "pdje_util_common.hpp"
using namespace godot;
void
PlayerWrapper::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("GetConsumedFrames"),
                         &PlayerWrapper::GetConsumedFrames);
    ClassDB::bind_method(D_METHOD("Activate"), &PlayerWrapper::Activate);
    ClassDB::bind_method(D_METHOD("Deactivate"), &PlayerWrapper::Deactivate);
    ClassDB::bind_method(D_METHOD("ChangeCursorPos", "framePos"),
                         &PlayerWrapper::ChangeCursorPos);
    ClassDB::bind_method(D_METHOD("GetStatus"), &PlayerWrapper::GetStatus);
    ClassDB::bind_method(D_METHOD("GetFXControlPanel"),
                         &PlayerWrapper::GetFXControlPanel);
    ClassDB::bind_method(D_METHOD("GetMusicControlPanel"),
                         &PlayerWrapper::GetMusicControlPanel);
}

void
PlayerWrapper::Init(audioPlayer *refobj, PDJE *refengine)
{
    playerobj = refobj;
    engineobj = refengine;
}

bool
PlayerWrapper::Activate()
{

    if (playerobj == nullptr) {
        return false;
    }
    return playerobj->Activate();
}

bool
PlayerWrapper::Deactivate()
{

    if (playerobj == nullptr) {
        return false;
    }
    return playerobj->Deactivate();
}

bool
PlayerWrapper::ChangeCursorPos(String pos)
{

    if (playerobj == nullptr)
        return false;

    auto str = GStrToCStr(pos);
    playerobj->ChangeCursorPos(std::stoull(str));

    return true;
}

String
PlayerWrapper::GetConsumedFrames()
{

    if (playerobj == nullptr)
        return String();

    auto frames = playerobj->GetConsumedFrames();

    auto frameString = std::to_string(frames);
    return CStrToGStr(frameString);
}

String
PlayerWrapper::GetStatus()
{
    return "This is Deprecated Function."; // Function Deprecated.
}

Ref<FXWrapper>
PlayerWrapper::GetFXControlPanel()
{
    auto ref = Ref<FXWrapper>(memnew(FXWrapper));
    if (playerobj == nullptr)
        return ref;
    ref->Init(playerobj->GetFXControlPanel());
    return ref;
}

Ref<MusPanelWrapper>
PlayerWrapper::GetMusicControlPanel()
{

    auto ref = Ref<MusPanelWrapper>(memnew(MusPanelWrapper));
    if (playerobj == nullptr)
        return ref;
    if (engineobj == nullptr)
        return ref;

    ref->Init((playerobj->GetMusicControlPanel()), engineobj);
    return ref;
}