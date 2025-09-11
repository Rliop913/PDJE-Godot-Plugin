#include "PDJE_Wrapper.hpp"
#include <filesystem>
#include <godot_cpp/core/class_db.hpp>
#include "fileNameSanitizer.hpp"
using namespace godot;

VARIANT_ENUM_CAST(PDJE_Wrapper::PDJE_PLAY_MODE);
void
PDJE_Wrapper::_bind_methods()
{
    BIND_ENUM_CONSTANT(FULL_MANUAL_RENDER);
    BIND_ENUM_CONSTANT(FULL_PRE_RENDER);
    BIND_ENUM_CONSTANT(HYBRID_RENDER);
    ClassDB::bind_method(
        D_METHOD("InitPlayer", "mode", "track_title", "buffer_size"),
        &PDJE_Wrapper::InitPlayer);
    ClassDB::bind_method(D_METHOD("SearchMusic", "Title", "composer", "bpm"),
                         &PDJE_Wrapper::SearchMusic,
                         DEFVAL(-1.0));
    ClassDB::bind_method(D_METHOD("SearchTrack", "Title"),
                         &PDJE_Wrapper::SearchTrack);
    ClassDB::bind_method(D_METHOD("InitEngine", "DBPath"),
                         &PDJE_Wrapper::InitEngine);
    ClassDB::bind_method(D_METHOD("GetPlayer"), &PDJE_Wrapper::GetPlayer);
    ClassDB::bind_method(
        D_METHOD("InitEditor", "authName", "authEmail", "projectRoot"),
        &PDJE_Wrapper::InitEditor);
    ClassDB::bind_method(D_METHOD("GetEditor"), &PDJE_Wrapper::GetEditor);
    ClassDB::bind_method(D_METHOD("CloseEditor"), &PDJE_Wrapper::CloseEditor);
    ClassDB::bind_method(D_METHOD("PullOutCoreLine"), &PDJE_Wrapper::PullOutCoreLine);
    ClassDB::bind_method(D_METHOD("GetNoteObjects", "trackTitle"), &PDJE_Wrapper::GetNoteObjects);
    
    ADD_SIGNAL(MethodInfo("note_gen_signal",
                          PropertyInfo(Variant::STRING, "note_type"),
                          PropertyInfo(Variant::STRING, "note_detail"),
                          PropertyInfo(Variant::STRING, "first_arg"),
                          PropertyInfo(Variant::STRING, "second_arg"),
                          PropertyInfo(Variant::STRING, "third_arg"),
                          PropertyInfo(Variant::STRING, "y_pos_start"),
                          PropertyInfo(Variant::STRING, "y_pos_end")));
                        
}

PDJE_Wrapper::PDJE_Wrapper()
{
}

PDJE_Wrapper::~PDJE_Wrapper()
{
    // Add your cleanup here.
}

Array
PDJE_Wrapper::SearchTrack(String Title)
{
    if (!engine.has_value()) {
        return Array();
    }
    auto res = engine->SearchTrack(GStrToCStr(Title));

    Array trackList;
    for (auto &track : res) {
        
        std::stringstream before_csv(track.cachedMixList);
        std::string sanitized_music_name;
        std::string unsanitized_csv;
        while(std::getline(before_csv, sanitized_music_name, ',')){
            unsanitized_csv += PDJE_Name_Sanitizer::getFileName(sanitized_music_name);
            unsanitized_csv += ',';
        }
        if(!unsanitized_csv.empty()){
            unsanitized_csv.pop_back();
        }
        Dictionary trackMetaData;
        trackMetaData["title"]  = CStrToGStr(PDJE_Name_Sanitizer::getFileName(track.trackTitle));
        trackMetaData["mixSet"] = CStrToGStr(unsanitized_csv);
        trackList.push_back(trackMetaData);
    }
    return trackList;
}
Array
PDJE_Wrapper::SearchMusic(String Title, String composer, double bpm)
{
    if (!engine.has_value()) {
        return Array();
    }
    auto res =
        engine->SearchMusic(GStrToCStr(Title), GStrToCStr(composer), bpm);

    Array musicList;
    for (auto &music : res) {
        Dictionary musicMetaData;
        
        
        musicMetaData["title"]     = CStrToGStr(PDJE_Name_Sanitizer::getFileName(music.title));
        musicMetaData["composer"]  = CStrToGStr(PDJE_Name_Sanitizer::getFileName(music.composer));
        musicMetaData["bpm"]       = music.bpm;
        musicMetaData["firstBar"]  = CStrToGStr(music.firstBeat);
        musicMetaData["musicPath"] = CStrToGStr(music.musicPath);
        musicList.push_back(musicMetaData);
    }
    return musicList;
}

bool
PDJE_Wrapper::InitPlayer(PDJE_PLAY_MODE mode,
                         String         trackTitle,
                         unsigned int   FrameBufferSize)
{
    if (!engine.has_value()) {
        return false;
    }
    auto td = engine->SearchTrack(GStrToCStr(trackTitle));
    if (td.empty() && mode != PDJE_PLAY_MODE::FULL_MANUAL_RENDER) {
        return false;
    }
    PLAY_MODE pm;
    switch (mode) {
    case PDJE_PLAY_MODE::FULL_PRE_RENDER:
        pm = PLAY_MODE::FULL_PRE_RENDER;
        break;
    case PDJE_PLAY_MODE::HYBRID_RENDER:
        pm = PLAY_MODE::HYBRID_RENDER;
        break;
    case PDJE_PLAY_MODE::FULL_MANUAL_RENDER:
        pm = PLAY_MODE::FULL_MANUAL_RENDER;
        td.emplace_back();
        break;
    default:
        return false;
    }

    return engine->InitPlayer(pm, td.front(), FrameBufferSize);
}

bool
PDJE_Wrapper::InitEngine(String DBPath)
{

    engine.emplace(GpathToCPath(DBPath).string());
    return engine.has_value();
}

bool
PDJE_Wrapper::InitEditor(String authName, String authEmail, String projectRoot)
{

    if (!engine.has_value()) {
        return false;
    }

    return engine->InitEditor(GStrToCStr(authName),
                              GStrToCStr(authEmail),
                              GpathToCPath(projectRoot).string());
}

Ref<PlayerWrapper>
PDJE_Wrapper::GetPlayer()
{
    auto ref = Ref<PlayerWrapper>(memnew(PlayerWrapper));
    if (!engine->player) {
        return ref;
    }
    ref->Init(engine->player.get(), &engine.value());
    return ref;
}

Ref<EditorWrapper>
PDJE_Wrapper::GetEditor()
{
    auto ref = Ref<EditorWrapper>(memnew(EditorWrapper));
    if (!engine->editor) {
        return ref;
    }

    ref->Init(engine->editor.get(), &engine.value());
    return ref;
}

void
PDJE_Wrapper::ResetPlayer()
{
    engine->ResetPlayer();
}
void
PDJE_Wrapper::_ready()
{
}

void
PDJE_Wrapper::_process(double delta)
{
}

void
PDJE_Wrapper::CloseEditor()
{
    engine->CloseEditor();
}

bool
PDJE_Wrapper::GetNoteObjects(String trackTitle)
{
    auto searched = engine->SearchTrack(GStrToCStr(trackTitle));
    auto track    = searched.front();

    OBJ_SETTER_CALLBACK osc = [this](std::string        note_type,
                                     std::string        note_detail,
                                     std::string        first_arg,
                                     std::string        second_arg,
                                     std::string        third_arg,
                                     unsigned long long y_pos_start,
                                     unsigned long long y_pos_end) {
        call_deferred("emit_signal",
                      "note_gen_signal",
                      CStrToGStr(note_type),
                      CStrToGStr(note_detail),
                      CStrToGStr(first_arg),
                      CStrToGStr(second_arg),
                      CStrToGStr(third_arg),
                      String::num_uint64(y_pos_start),
                      String::num_uint64(y_pos_end));
    };
    return engine->GetNoteObjects(track, osc);
}

Ref<CoreLine>
PDJE_Wrapper::PullOutCoreLine()
{
    auto ref = Ref<CoreLine>(memnew(CoreLine));
    if (!engine->editor) {
        return ref;
    }
    auto line = engine->PullOutDataLine();
    ref->Init(line);
    return ref;
}