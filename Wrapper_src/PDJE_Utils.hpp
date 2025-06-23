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
std::u8string 
GStrToCUStr(const String& gstr)
{
    auto gbin = gstr.to_utf8_buffer();
	return std::u8string(reinterpret_cast<const char8_t*>(gbin.ptr()),gbin.size());
}
inline 
String 
CStrToGStr(const std::string& cstr)
{
    return godot::String::utf8(cstr.c_str());
}

inline 
String 
CStrToGStr(const std::u8string& cstr)
{
    auto safeStr = std::string(cstr.begin(), cstr.end());
    return CStrToGStr(safeStr);
}

inline
std::filesystem::path
GpathToCPath(const String& gpath)
{
    auto GlobalGpath = ProjectSettings::get_singleton()->globalize_path(gpath);
    std::u8string Cpath = GStrToCUStr(GlobalGpath);
    return std::filesystem::path(Cpath);
}
