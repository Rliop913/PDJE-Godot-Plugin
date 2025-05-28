#pragma once

#include <godot_cpp/classes/ref.hpp>

#include "PDJE_interface.hpp"

namespace godot {

class PlayerWrapper : public RefCounted{
	GDCLASS(PlayerWrapper, RefCounted)
private:
	audioPlayer* playerobj = nullptr;
protected:
	static void _bind_methods();

public:
	bool Activate();
	bool Deactivate();
	// GetFXControlPannel
	// GetMusicControlPannel
	bool ChangeCursorPos(String pos);
	String GetConsumedFrames();
	String GetStatus();
	void Init(audioPlayer* refobj);
	PlayerWrapper() = default;
	~PlayerWrapper() = default;
};
}