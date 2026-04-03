#include "util/db/nearest/PDJE_VectorTypes.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

using namespace godot;

void
PDJE_VectorItem::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_id", "id"), &PDJE_VectorItem::set_id);
    ClassDB::bind_method(D_METHOD("get_id"), &PDJE_VectorItem::get_id);
    ClassDB::bind_method(D_METHOD("set_embedding", "embedding"),
                         &PDJE_VectorItem::set_embedding);
    ClassDB::bind_method(D_METHOD("get_embedding"),
                         &PDJE_VectorItem::get_embedding);
    ClassDB::bind_method(D_METHOD("set_text_payload", "text_payload"),
                         &PDJE_VectorItem::set_text_payload);
    ClassDB::bind_method(D_METHOD("get_text_payload"),
                         &PDJE_VectorItem::get_text_payload);
    ClassDB::bind_method(D_METHOD("set_bytes_payload", "bytes_payload"),
                         &PDJE_VectorItem::set_bytes_payload);
    ClassDB::bind_method(D_METHOD("get_bytes_payload"),
                         &PDJE_VectorItem::get_bytes_payload);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "id"), "set_id", "get_id");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "embedding"),
                 "set_embedding",
                 "get_embedding");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "text_payload"),
                 "set_text_payload",
                 "get_text_payload");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes_payload"),
                 "set_bytes_payload",
                 "get_bytes_payload");
}

void
PDJE_VectorItem::set_id(const String &id)
{
    id_ = id;
}

String
PDJE_VectorItem::get_id() const
{
    return id_;
}

void
PDJE_VectorItem::set_embedding(const PackedFloat32Array &embedding)
{
    embedding_ = embedding;
}

PackedFloat32Array
PDJE_VectorItem::get_embedding() const
{
    return embedding_;
}

void
PDJE_VectorItem::set_text_payload(const String &text_payload)
{
    text_payload_ = text_payload;
}

String
PDJE_VectorItem::get_text_payload() const
{
    return text_payload_;
}

void
PDJE_VectorItem::set_bytes_payload(const PackedByteArray &bytes_payload)
{
    bytes_payload_ = bytes_payload;
}

PackedByteArray
PDJE_VectorItem::get_bytes_payload() const
{
    return bytes_payload_;
}

void
PDJE_VectorHit::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_id", "id"), &PDJE_VectorHit::set_id);
    ClassDB::bind_method(D_METHOD("get_id"), &PDJE_VectorHit::get_id);
    ClassDB::bind_method(D_METHOD("set_distance", "distance"),
                         &PDJE_VectorHit::set_distance);
    ClassDB::bind_method(D_METHOD("get_distance"),
                         &PDJE_VectorHit::get_distance);
    ClassDB::bind_method(D_METHOD("set_text_payload", "text_payload"),
                         &PDJE_VectorHit::set_text_payload);
    ClassDB::bind_method(D_METHOD("get_text_payload"),
                         &PDJE_VectorHit::get_text_payload);
    ClassDB::bind_method(D_METHOD("set_bytes_payload", "bytes_payload"),
                         &PDJE_VectorHit::set_bytes_payload);
    ClassDB::bind_method(D_METHOD("get_bytes_payload"),
                         &PDJE_VectorHit::get_bytes_payload);

    ADD_PROPERTY(PropertyInfo(Variant::STRING, "id"), "set_id", "get_id");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "distance"),
                 "set_distance",
                 "get_distance");
    ADD_PROPERTY(PropertyInfo(Variant::STRING, "text_payload"),
                 "set_text_payload",
                 "get_text_payload");
    ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "bytes_payload"),
                 "set_bytes_payload",
                 "get_bytes_payload");
}

void
PDJE_VectorHit::set_id(const String &id)
{
    id_ = id;
}

String
PDJE_VectorHit::get_id() const
{
    return id_;
}

void
PDJE_VectorHit::set_distance(double distance)
{
    distance_ = distance;
}

double
PDJE_VectorHit::get_distance() const
{
    return distance_;
}

void
PDJE_VectorHit::set_text_payload(const String &text_payload)
{
    text_payload_ = text_payload;
}

String
PDJE_VectorHit::get_text_payload() const
{
    return text_payload_;
}

void
PDJE_VectorHit::set_bytes_payload(const PackedByteArray &bytes_payload)
{
    bytes_payload_ = bytes_payload;
}

PackedByteArray
PDJE_VectorHit::get_bytes_payload() const
{
    return bytes_payload_;
}
