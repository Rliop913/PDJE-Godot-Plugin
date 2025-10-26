#include "FXWrapper.hpp"
#include "FXArgWrapper.hpp"
#include "PDJE_Utils.hpp"
using namespace godot;

void
FXWrapper::Init(FXControlPanel *refobj)
{
    try {
        fxpanel = refobj;
    } catch (const std::exception &e) {
        print_line(CStrToGStr(e.what()));
    }
}

void
FXWrapper::_bind_methods()
{
    try {
        ClassDB::bind_method(D_METHOD("CheckFXOn"), &FXWrapper::CheckFXOn);
        ClassDB::bind_method(D_METHOD("FX_ON_OFF", "fx", "onoff"),
                             &FXWrapper::FX_ON_OFF);
        ClassDB::bind_method(D_METHOD("GetArgSetter"),
                             &FXWrapper::GetArgSetter);
    } catch (const std::exception &e) {
        print_line(CStrToGStr(e.what()));
    }
}

bool
FXWrapper::CheckFXOn()
{
    try {
        if (fxpanel == nullptr)
            return false;
        return fxpanel->checkSomethingOn();
    } catch (const std::exception &e) {
        print_line(CStrToGStr(e.what()));
        return false;
    }
}

bool
FXWrapper::FX_ON_OFF(int fx, bool onoff)
{
    try {
        if (fxpanel == nullptr)
            return false;
        FXList ofx = static_cast<FXList>(fx);
        fxpanel->FX_ON_OFF(ofx, onoff);
        return true;
    } catch (const std::exception &e) {
        print_line(CStrToGStr(e.what()));
        return false;
    }
}

Ref<FXArgWrapper>
FXWrapper::GetArgSetter()
{
    try {
        auto ref = Ref<FXArgWrapper>(memnew(FXArgWrapper));
        if (fxpanel == nullptr)
            return ref;
        ref->Init(fxpanel);
        return ref;
    } catch (const std::exception &e) {
        print_line(CStrToGStr(e.what()));
        return Ref<FXArgWrapper>();
    }
}