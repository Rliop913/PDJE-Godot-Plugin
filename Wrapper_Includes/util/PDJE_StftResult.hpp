#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

namespace godot {

class PDJE_StftResult : public RefCounted {
    GDCLASS(PDJE_StftResult, RefCounted)

  private:
    PackedFloat32Array real_;
    PackedFloat32Array imag_;
    PackedFloat32Array power_;

  protected:
    static void
    _bind_methods();

  public:
    void
    set_real(const PackedFloat32Array &real);
    PackedFloat32Array
    get_real() const;

    void
    set_imag(const PackedFloat32Array &imag);
    PackedFloat32Array
    get_imag() const;

    void
    set_power(const PackedFloat32Array &power);
    PackedFloat32Array
    get_power() const;
};

} // namespace godot
