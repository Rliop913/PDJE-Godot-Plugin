#include "FXWrapper.hpp"

using namespace godot;

void
FXWrapper::Init(FXControlPanel *refobj)
{
    fxpanel = refobj;
}

void
FXWrapper::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("CheckFXOn"), &FXWrapper::CheckFXOn);
    ClassDB::bind_method(D_METHOD("FX_ON_OFF", "fx", "onoff"),
                         &FXWrapper::FX_ON_OFF);
    ClassDB::bind_method(D_METHOD("GetArgSetter"), &FXWrapper::GetArgSetter);
}

bool
FXWrapper::CheckFXOn()
{
    if (fxpanel == nullptr)
        return false;
    return fxpanel->checkSomethingOn();
}

bool
FXWrapper::FX_ON_OFF(int fx, bool onoff)
{
    if (fxpanel == nullptr)
        return false;
    FXList ofx = static_cast<FXList>(fx);
    fxpanel->FX_ON_OFF(ofx, onoff);
    return true;
}

Ref<FXArgWrapper>
FXWrapper::GetArgSetter()
{
    auto ref = Ref<FXArgWrapper>(memnew(FXArgWrapper));
    if (fxpanel == nullptr)
        return ref;
    ref->Init(fxpanel);
    return ref;
}