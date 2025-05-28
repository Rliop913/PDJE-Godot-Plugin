#pragma once

#include <string>
#include <godot_cpp/variant/string.hpp>
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