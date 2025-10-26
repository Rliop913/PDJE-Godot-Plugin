#include "FXArgWrapper.hpp"
#include "PDJE_Utils.hpp"
#include "variant/array.hpp"
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
    try {
        refwrap.emplace(refobj);
    } catch (const std::exception &e) {
        print_line(CStrToGStr(e.what()));
    }
}

Array
FXArgWrapper::GetFXArgKeys(int fx)
{
    try {
        if (!refwrap.has_value())
            return Array();
        FXList ofx = static_cast<FXList>(fx);
        Array  usableKeys;
        for (auto &key : refwrap->GetFXArgKeys(ofx)) {
            usableKeys.push_back(CStrToGStr(key));
        }
        return usableKeys;
    } catch (const std::exception &e) {
        print_line(CStrToGStr(e.what()));
        return Array();
    }
}

bool
FXArgWrapper::SetFXArg(int fx, String key, double arg)
{
    try {
        if (!refwrap.has_value())
            return false;
        FXList ofx = static_cast<FXList>(fx);
        refwrap->SetFXArg(ofx, GStrToCStr(key), arg);
        return true;
    } catch (const std::exception &e) {
        print_line(CStrToGStr(e.what()));
        return false;
    }
}