#include "PDJE_Wrapper.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <filesystem>
using namespace godot;

VARIANT_ENUM_CAST(PDJE_Wrapper::PDJE_PLAY_MODE);
void PDJE_Wrapper::_bind_methods() {
	BIND_ENUM_CONSTANT(FULL_MANUAL_RENDER);
	BIND_ENUM_CONSTANT(FULL_PRE_RENDER);
	BIND_ENUM_CONSTANT(HYBRID_RENDER);
	ClassDB::bind_method(
		D_METHOD("InitPlayer", "mode", "track_title", "buffer_size"),
		&PDJE_Wrapper::InitPlayer);
	ClassDB::bind_method(
		D_METHOD("SearchMusic", "Title", "composer", "bpm"),
		&PDJE_Wrapper::SearchMusic);
	ClassDB::bind_method(
		D_METHOD("SearchTrack", "Title"),
		&PDJE_Wrapper::SearchTrack);
	ClassDB::bind_method(
		D_METHOD("InitEngine", "DBPath"),
		&PDJE_Wrapper::InitEngine);
	ClassDB::bind_method(
		D_METHOD("GetPlayer"),
		&PDJE_Wrapper::GetPlayer);
		
	
	
}

PDJE_Wrapper::PDJE_Wrapper()
{
	
}

PDJE_Wrapper::~PDJE_Wrapper() {
	// Add your cleanup here.
}


Array 
PDJE_Wrapper::SearchTrack(String Title)
{
	if(!engine.has_value()){
		return Array();
	}
	auto res =
	engine->SearchTrack(
		GStrToCStr(Title)
	);

	Array trackList;
	for(auto& track : res){
		Dictionary trackMetaData;
		trackMetaData["title"] = CStrToGStr(track.trackTitle);
		trackMetaData["mixSet"] = CStrToGStr(track.cachedMixList);
		trackList.push_back(trackMetaData);
	}
	return trackList;
}
Array
PDJE_Wrapper::SearchMusic(String Title, String composer, double bpm)
{
	if(!engine.has_value()){
		return Array();
	}
	auto res =
	engine->SearchMusic(
		GStrToCStr(Title),
		GStrToCStr(composer),
		bpm
	);
	
	Array musicList;
	for(auto& music : res){
		Dictionary musicMetaData;
		musicMetaData["title"] = CStrToGStr(music.title);
		musicMetaData["composer"] = CStrToGStr(music.composer);
		musicMetaData["bpm"] = music.bpm;
		musicMetaData["firstBar"] = CStrToGStr(music.firstBar);
		musicMetaData["musicPath"] = CStrToGStr(music.musicPath);
		musicList.push_back(musicMetaData);
	}
	return musicList;
}

bool 
PDJE_Wrapper::InitPlayer(PDJE_PLAY_MODE mode, String trackTitle, unsigned int FrameBufferSize)
{
	if(!engine.has_value()){
		return false;
	}
	auto td = engine->SearchTrack(GStrToCStr(trackTitle));
	if(td.empty() && mode != PDJE_PLAY_MODE::FULL_MANUAL_RENDER){
		return false;
	}
	PLAY_MODE pm;
	switch (mode)
	{
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

	
	return
	engine->InitPlayer(
		pm,
		td.front(),
		FrameBufferSize
	);
}

bool
PDJE_Wrapper::InitEngine(String DBPath)
{
	auto gpath = ProjectSettings::get_singleton()->globalize_path(DBPath);
	
	engine.emplace(
		GStrToCStr(gpath)
	);
	return engine.has_value();
}

bool
PDJE_Wrapper::InitEditor(String authName, String authEmail, String projectRoot)
{

	if(!engine.has_value()){
		return false;
	}
	auto gpath = ProjectSettings::get_singleton()->globalize_path(projectRoot);
	return
	engine->InitEditor(
		GStrToCStr(authName),
		GStrToCStr(authEmail),
		GStrToCStr(gpath)
	);
}

Ref<PlayerWrapper>
PDJE_Wrapper::GetPlayer()
{
	auto ref =  Ref<PlayerWrapper>(memnew(PlayerWrapper));
	if(!engine->player.has_value()){
		return ref;
	}
	ref->Init(&(engine->player.value()), &engine.value());
	return ref;
}

Ref<EditorWrapper>
PDJE_Wrapper::GetEditor()
{
	auto ref =  Ref<EditorWrapper>(memnew(EditorWrapper));
	if(!engine->editor.has_value()){
		return ref;
	}
	
	ref->Init(&(engine->editor.value()), &engine.value());
	return ref;
}
void
PDJE_Wrapper::_ready()
{
	
}

void 
PDJE_Wrapper::_process(double delta) {
	
}