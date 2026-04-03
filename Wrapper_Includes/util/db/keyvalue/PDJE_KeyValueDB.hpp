#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <memory>

namespace godot {

class PDJE_MIR;

namespace pdje_keyvalue_db_internal {
struct State;
}

class PDJE_KeyValueDB : public Node {
    GDCLASS(PDJE_KeyValueDB, Node)

  private:
    friend class PDJE_MIR;

    std::unique_ptr<pdje_keyvalue_db_internal::State> state_;

    bool
    TryGetBytesSilently(String key,
                        PackedByteArray &out_value,
                        bool            &out_found) const;

  protected:
    static void
    _bind_methods();

  public:
    bool
    Create(String path, bool truncate_if_exists = false);

    bool
    Destroy(String path);

    bool
    Open(String path,
         bool   create_if_missing  = false,
         bool   truncate_if_exists = false,
         bool   read_only          = false);

    bool
    Close();

    bool
    IsOpen() const;

    String
    GetPath() const;

    bool
    Contains(String key);

    String
    GetText(String key);

    PackedByteArray
    GetBytes(String key);

    bool
    PutText(String key, String value);

    bool
    PutBytes(String key, PackedByteArray value);

    bool
    Erase(String key);

    PackedStringArray
    ListKeys(String prefix = "");

    PDJE_KeyValueDB();
    ~PDJE_KeyValueDB();

    void
    _ready() override;
};

} // namespace godot
