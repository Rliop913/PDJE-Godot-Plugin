#include "PlayerWrapper.hpp"
#include "PDJE_Utils.hpp"
using namespace godot;
void 
PlayerWrapper::_bind_methods() {
    ClassDB::bind_method(
		D_METHOD("GetConsumedFrames"),
		&PlayerWrapper::GetConsumedFrames);
	ClassDB::bind_method(
		D_METHOD("Activate"),
		&PlayerWrapper::Activate);
	ClassDB::bind_method(
		D_METHOD("Deactivate"),
		&PlayerWrapper::Deactivate);
	ClassDB::bind_method(
		D_METHOD("ChangeCursorPos", "framePos"),
		&PlayerWrapper::ChangeCursorPos);
	ClassDB::bind_method(
		D_METHOD("GetStatus"),
		&PlayerWrapper::GetStatus);
}

void
PlayerWrapper::Init(audioPlayer* refobj)
{
    playerobj = refobj;
    
}

bool 
PlayerWrapper::Activate()
{
    if(playerobj == nullptr){
        return false;
    }
    return playerobj->Activate();
}

bool 
PlayerWrapper::Deactivate()
{
    if(playerobj == nullptr){
        return false;
    }
    return playerobj->Deactivate();
}


bool
PlayerWrapper::ChangeCursorPos(String pos)
{
    if(playerobj == nullptr) return false;

    auto str = GStrToCStr(pos);
    try{
        playerobj->ChangeCursorPos(
            std::stoull(str)
        );
    }
    catch(...){
        return false;
    }
    return true;
}

String
PlayerWrapper::GetConsumedFrames()
{
    if(playerobj == nullptr) return String();

    auto frames = playerobj->GetConsumedFrames();
    try{
        auto frameString = std::to_string(frames);
        return CStrToGStr(frameString);
    }
    catch(...){
        return String();
    }
}


String
PlayerWrapper::GetStatus()
{
    if(playerobj == nullptr) return String();

    return CStrToGStr(playerobj->STATUS);
}