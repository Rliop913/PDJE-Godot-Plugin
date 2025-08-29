#include "MusPannelWrapper.hpp"
#include "PDJE_Utils.hpp"
#include "PDJE_Wrapper.hpp"
using namespace godot;

void
MusPannelWrapper::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("LoadMusic", "Title", "composer", "bpm"),
                         &MusPannelWrapper::LoadMusic);
    ClassDB::bind_method(D_METHOD("CueMusic", "title", "newPos"),
                         &MusPannelWrapper::CueMusic);
    ClassDB::bind_method(D_METHOD("SetMusic", "title", "onoff"),
                         &MusPannelWrapper::SetMusic);
    ClassDB::bind_method(D_METHOD("GetLoadedMusicList"),
                         &MusPannelWrapper::GetLoadedMusicList);
    ClassDB::bind_method(D_METHOD("UnloadMusic", "title"),
                         &MusPannelWrapper::UnloadMusic);

    ClassDB::bind_method(D_METHOD("getFXHandle", "title"),
                         &MusPannelWrapper::getFXHandle);

    ClassDB::bind_method(
        D_METHOD("ChangeBpm", "title", "targetBpm", "originBpm"),
        &MusPannelWrapper::ChangeBpm);
}

void
MusPannelWrapper::Init(MusicControlPannel *refobj, PDJE *refengine)
{
    musref = refobj;
    engine = refengine;
}

Ref<FXWrapper>
MusPannelWrapper::getFXHandle(String title)
{
    auto ref = Ref<FXWrapper>(memnew(FXWrapper));
    if (musref == nullptr)
        return ref;

    ref->Init(musref->getFXHandle(GStrToCStr(title)));
    return ref;
}

bool
MusPannelWrapper::CueMusic(String title, String newPos)
{
    if (musref == nullptr)
        return false;

    try {
        const unsigned long long posull = std::stoull(GStrToCStr(newPos));
        return musref->CueMusic(GStrToCStr(title), posull);
    } catch (...) {
        return false;
    }
}

bool
MusPannelWrapper::SetMusic(String title, const bool onOff)
{
    if (musref == nullptr)
        return false;
    return musref->SetMusic(GStrToCStr(title), onOff);
}

bool
MusPannelWrapper::UnloadMusic(String title)
{
    if (musref == nullptr)
        return false;
    return musref->UnloadMusic(GStrToCStr(title));
}

Array
MusPannelWrapper::GetLoadedMusicList()
{
    if (musref == nullptr)
        return Array();
    auto  res = musref->GetLoadedMusicList();
    Array lists;
    for (auto &musicTitle : res) {
        lists.push_back(CStrToGStr(musicTitle));
    }
    return lists;
}

int
MusPannelWrapper::LoadMusic(String Title, String composer, double bpm)
{
    if (musref == nullptr)
        return -1;
    if (engine == nullptr)
        return -1;
    auto muslist =
        engine->SearchMusic(GStrToCStr(Title), GStrToCStr(composer), bpm);
    if (muslist.empty())
        return -2;
    return musref->LoadMusic((*engine->DBROOT.get()), muslist.front());
}

bool
MusPannelWrapper::ChangeBpm(String title, double targetBpm, double originBpm)
{
    if (musref == nullptr)
        return false;
    if (engine == nullptr)
        return false;

    return musref->ChangeBpm(GStrToCStr(title), targetBpm, originBpm);
}