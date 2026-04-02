#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <memory>

namespace godot {

namespace pdje_vector_db_internal {
struct State;
}

class PDJE_VectorHit;
class PDJE_VectorItem;

class PDJE_VectorDB : public Node {
    GDCLASS(PDJE_VectorDB, Node)

  private:
    std::unique_ptr<pdje_vector_db_internal::State> state_;

  protected:
    static void
    _bind_methods();

  public:
    bool
    Create(String root_path,
           int    dimension,
           int    trees              = 10,
           bool   prefault           = false,
           bool   truncate_if_exists = false);

    bool
    Destroy(String root_path,
            int    dimension,
            int    trees    = 10,
            bool   prefault = false);

    bool
    Open(String root_path,
         int    dimension,
         int    trees              = 10,
         bool   prefault           = false,
         bool   create_if_missing  = false,
         bool   truncate_if_exists = false,
         bool   read_only          = false);

    bool
    Close();

    bool
    IsOpen() const;

    String
    GetRootPath() const;

    int
    GetDimension() const;

    bool
    Contains(String id);

    Ref<PDJE_VectorItem>
    GetItem(String id);

    bool
    UpsertItem(Ref<PDJE_VectorItem> item);

    bool
    EraseItem(String id);

    Array
    Search(PackedFloat32Array query_embedding,
           int                limit    = 10,
           int                search_k = -1);

    PackedStringArray
    ListKeys();

    PDJE_VectorDB();
    ~PDJE_VectorDB();

    void
    _ready() override;
};

} // namespace godot
