#pragma once

#include <string>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <filesystem>
using namespace godot;

inline 
std::string 
GStrToCStr(const String& gstr)
{
    auto gbin = gstr.to_utf8_buffer();
	return std::string(reinterpret_cast<const char*>(gbin.ptr()),gbin.size());
}

inline 
String 
CStrToGStr(const std::string& cstr)
{
    return godot::String::utf8(cstr.c_str());
}


inline
std::filesystem::path
GpathToCPath(const String& gpath)
{
    auto GlobalGpath = ProjectSettings::get_singleton()->globalize_path(gpath);
    std::string Cpath = GStrToCStr(GlobalGpath);
    return std::filesystem::path(Cpath);
}
