#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <memory>

namespace godot {

namespace pdje_relational_db_internal {
struct State;
}

class PDJE_RelationalExecResult;

class PDJE_RelationalDB : public Node {
    GDCLASS(PDJE_RelationalDB, Node)

  private:
    std::unique_ptr<pdje_relational_db_internal::State> state_;

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

    Ref<PDJE_RelationalExecResult>
    Execute(String sql, Array params = Array());

    Array
    Query(String sql, Array params = Array());

    bool
    BeginTransaction();

    bool
    Commit();

    bool
    Rollback();

    PDJE_RelationalDB();
    ~PDJE_RelationalDB();

    void
    _ready() override;
};

} // namespace godot
