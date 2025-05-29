#pragma once
#include <godot_cpp/classes/node.hpp>
#include "PDJE_interface.hpp"
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/ref.hpp>
#include "PDJE_Utils.hpp"
#include "PlayerWrapper.hpp"
#include "MusPannelWrapper.hpp"
#include "EditorWrapper.hpp"
// #include "PDJE_interface.hpp"

namespace godot {



class PDJE_Wrapper : public Node {
	GDCLASS(PDJE_Wrapper, Node)

private:
	// std::string GStrToCStr(const String& gstr);
	// String CStrToGStr(const std::string& cstr);
	std::optional<PDJE> engine;

protected:
	static void _bind_methods();

public:
	enum PDJE_PLAY_MODE{
		FULL_PRE_RENDER = 0,
		HYBRID_RENDER,
		FULL_MANUAL_RENDER
	};
	bool InitEngine(String DBPath);
	bool InitEditor(String authName, String authEmail, String projectRoot);
	Array SearchMusic(String Title, String composer, double bpm = -1.0);
	Array SearchTrack(String Title);
	bool InitPlayer(PDJE_PLAY_MODE mode, String trackTitle, unsigned int FrameBufferSize);
	Ref<PlayerWrapper> GetPlayer();
	Ref<EditorWrapper> GetEditor();
	PDJE_Wrapper();
	~PDJE_Wrapper();

	void _ready() override;
	void _process(double delta) override;
};

}
