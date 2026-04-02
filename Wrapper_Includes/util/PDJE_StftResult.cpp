#include "util/PDJE_StftResult.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

using namespace godot;

void
PDJE_StftResult::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_real", "real"),
                         &PDJE_StftResult::set_real);
    ClassDB::bind_method(D_METHOD("get_real"), &PDJE_StftResult::get_real);
    ClassDB::bind_method(D_METHOD("set_imag", "imag"),
                         &PDJE_StftResult::set_imag);
    ClassDB::bind_method(D_METHOD("get_imag"), &PDJE_StftResult::get_imag);
    ClassDB::bind_method(D_METHOD("set_power", "power"),
                         &PDJE_StftResult::set_power);
    ClassDB::bind_method(D_METHOD("get_power"), &PDJE_StftResult::get_power);

    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "real"),
                 "set_real",
                 "get_real");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "imag"),
                 "set_imag",
                 "get_imag");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "power"),
                 "set_power",
                 "get_power");
}

void
PDJE_StftResult::set_real(const PackedFloat32Array &real)
{
    real_ = real;
}

PackedFloat32Array
PDJE_StftResult::get_real() const
{
    return real_;
}

void
PDJE_StftResult::set_imag(const PackedFloat32Array &imag)
{
    imag_ = imag;
}

PackedFloat32Array
PDJE_StftResult::get_imag() const
{
    return imag_;
}

void
PDJE_StftResult::set_power(const PackedFloat32Array &power)
{
    power_ = power;
}

PackedFloat32Array
PDJE_StftResult::get_power() const
{
    return power_;
}
