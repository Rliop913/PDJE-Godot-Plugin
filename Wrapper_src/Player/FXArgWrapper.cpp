#include "FXArgWrapper.hpp"
#include "PDJE_Utils.hpp"
using namespace godot;

void
FXArgWrapper::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("GetFXArgKeys", "fx"),
                         &FXArgWrapper::GetFXArgKeys);
    ClassDB::bind_method(D_METHOD("SetFXArg", "fx", "key", "arg"),
                         &FXArgWrapper::SetFXArg);
}

void
FXArgWrapper::Init(FXControlPanel *refobj)
{
    refwrap.emplace(refobj);
}

Array
FXArgWrapper::GetFXArgKeys(int fx)
{
    if (!refwrap.has_value())
        return Array();
    FXList ofx = static_cast<FXList>(fx);
    Array  usableKeys;
    for (auto &key : refwrap->GetFXArgKeys(ofx)) {
        usableKeys.push_back(CStrToGStr(key));
    }
    return usableKeys;
}

bool
FXArgWrapper::SetFXArg(int fx, String key, double arg)
{
    if (!refwrap.has_value())
        return false;
    FXList ofx = static_cast<FXList>(fx);
    refwrap->SetFXArg(ofx, GStrToCStr(key), arg);
    return true;
}