#include "PlayerWrapper.hpp"
#include "FXWrapper.hpp"
#include "MusPanelWrapper.hpp"
#include "PDJE_Utils.hpp"
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
    PDJE_DEFAULT_TRY_CATCH(playerobj = refobj; engineobj = refengine;, ;)
}

bool
PlayerWrapper::Activate()
{
    PDJE_DEFAULT_TRY_CATCH(
        if (playerobj == nullptr) {
            return false;
        } return playerobj->Activate();
        , return false;)
}

bool
PlayerWrapper::Deactivate()
{
    PDJE_DEFAULT_TRY_CATCH(
        if (playerobj == nullptr) {
            return false;
        } return playerobj->Deactivate();
        , return false;)
}

bool
PlayerWrapper::ChangeCursorPos(String pos)
{
    PDJE_DEFAULT_TRY_CATCH(
        if (playerobj == nullptr) return false;

        auto str = GStrToCStr(pos);
        try { playerobj->ChangeCursorPos(std::stoull(str)); } catch (...) {
            return false;
        } return true;
        , return false;)
}

String
PlayerWrapper::GetConsumedFrames()
{
    PDJE_DEFAULT_TRY_CATCH(
        if (playerobj == nullptr) return String();

        auto frames = playerobj->GetConsumedFrames();
        try {
            auto frameString = std::to_string(frames);
            return CStrToGStr(frameString);
        } catch (...) { return String(); },
        return "Exception:" + CStrToGStr(e.what());)
}

String
PlayerWrapper::GetStatus()
{
    PDJE_DEFAULT_TRY_CATCH(if (playerobj == nullptr) return String();
                           return CStrToGStr(playerobj->STATUS);
                           , return CStrToGStr(e.what());)
}

Ref<FXWrapper>
PlayerWrapper::GetFXControlPanel()
{
    PDJE_DEFAULT_TRY_CATCH(auto ref = Ref<FXWrapper>(memnew(FXWrapper));
                           if (playerobj == nullptr) return ref;
                           ref->Init(playerobj->GetFXControlPanel());
                           return ref;
                           , return Ref<FXWrapper>();)
}

Ref<MusPanelWrapper>
PlayerWrapper::GetMusicControlPanel()
{
    PDJE_DEFAULT_TRY_CATCH(
        auto ref = Ref<MusPanelWrapper>(memnew(MusPanelWrapper));
        if (playerobj == nullptr) return ref;
        if (engineobj == nullptr) return ref;

        ref->Init((playerobj->GetMusicControlPanel()), engineobj);
        return ref;
        , return Ref<MusPanelWrapper>();)
}