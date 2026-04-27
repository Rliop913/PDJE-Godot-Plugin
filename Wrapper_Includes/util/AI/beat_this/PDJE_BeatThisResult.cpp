#include "util/AI/beat_this/PDJE_BeatThisResult.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>
#include <godot_cpp/variant/variant.hpp>

using namespace godot;

void
PDJE_BeatThisResult::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_beats", "beats"),
                         &PDJE_BeatThisResult::set_beats);
    ClassDB::bind_method(D_METHOD("get_beats"),
                         &PDJE_BeatThisResult::get_beats);
    ClassDB::bind_method(D_METHOD("set_downbeats", "downbeats"),
                         &PDJE_BeatThisResult::set_downbeats);
    ClassDB::bind_method(D_METHOD("get_downbeats"),
                         &PDJE_BeatThisResult::get_downbeats);

    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT64_ARRAY, "beats"),
                 "set_beats",
                 "get_beats");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT64_ARRAY, "downbeats"),
                 "set_downbeats",
                 "get_downbeats");
}

void
PDJE_BeatThisResult::set_beats(const PackedFloat64Array &beats)
{
    beats_ = beats;
}

PackedFloat64Array
PDJE_BeatThisResult::get_beats() const
{
    return beats_;
}

void
PDJE_BeatThisResult::set_downbeats(const PackedFloat64Array &downbeats)
{
    downbeats_ = downbeats;
}

PackedFloat64Array
PDJE_BeatThisResult::get_downbeats() const
{
    return downbeats_;
}
