#pragma once

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_float64_array.hpp>

namespace godot {

class PDJE_BeatThisResult : public RefCounted {
    GDCLASS(PDJE_BeatThisResult, RefCounted)

  private:
    PackedFloat64Array beats_;
    PackedFloat64Array downbeats_;

  protected:
    static void
    _bind_methods();

  public:
    void
    set_beats(const PackedFloat64Array &beats);
    PackedFloat64Array
    get_beats() const;

    void
    set_downbeats(const PackedFloat64Array &downbeats);
    PackedFloat64Array
    get_downbeats() const;
};

} // namespace godot
