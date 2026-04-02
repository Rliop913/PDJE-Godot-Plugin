#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <cstdint>

namespace godot {

class PDJE_RelationalRow : public RefCounted {
    GDCLASS(PDJE_RelationalRow, RefCounted)

  private:
    PackedStringArray columns_;
    Array             values_;

  protected:
    static void
    _bind_methods();

  public:
    void
    set_columns(const PackedStringArray &columns);
    PackedStringArray
    get_columns() const;

    void
    set_values(const Array &values);
    Array
    get_values() const;
};

class PDJE_RelationalExecResult : public RefCounted {
    GDCLASS(PDJE_RelationalExecResult, RefCounted)

  private:
    int64_t affected_rows_         = 0;
    bool    has_last_insert_rowid_ = false;
    int64_t last_insert_rowid_     = 0;

  protected:
    static void
    _bind_methods();

  public:
    void
    set_affected_rows(int64_t affected_rows);
    int64_t
    get_affected_rows() const;

    void
    set_has_last_insert_rowid(bool has_last_insert_rowid);
    bool
    get_has_last_insert_rowid() const;

    void
    set_last_insert_rowid(int64_t last_insert_rowid);
    int64_t
    get_last_insert_rowid() const;
};

} // namespace godot
