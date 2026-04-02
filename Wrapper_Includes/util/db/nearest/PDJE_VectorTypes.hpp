#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class PDJE_VectorItem : public RefCounted {
    GDCLASS(PDJE_VectorItem, RefCounted)

  private:
    String             id_;
    PackedFloat32Array embedding_;
    String             text_payload_;
    PackedByteArray    bytes_payload_;

  protected:
    static void
    _bind_methods();

  public:
    void
    set_id(const String &id);
    String
    get_id() const;

    void
    set_embedding(const PackedFloat32Array &embedding);
    PackedFloat32Array
    get_embedding() const;

    void
    set_text_payload(const String &text_payload);
    String
    get_text_payload() const;

    void
    set_bytes_payload(const PackedByteArray &bytes_payload);
    PackedByteArray
    get_bytes_payload() const;
};

class PDJE_VectorHit : public RefCounted {
    GDCLASS(PDJE_VectorHit, RefCounted)

  private:
    String          id_;
    double          distance_ = 0.0;
    String          text_payload_;
    PackedByteArray bytes_payload_;

  protected:
    static void
    _bind_methods();

  public:
    void
    set_id(const String &id);
    String
    get_id() const;

    void
    set_distance(double distance);
    double
    get_distance() const;

    void
    set_text_payload(const String &text_payload);
    String
    get_text_payload() const;

    void
    set_bytes_payload(const PackedByteArray &bytes_payload);
    PackedByteArray
    get_bytes_payload() const;
};

} // namespace godot
