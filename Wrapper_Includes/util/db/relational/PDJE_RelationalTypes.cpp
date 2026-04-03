#include "util/db/relational/PDJE_RelationalTypes.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/property_info.hpp>

using namespace godot;

void
PDJE_RelationalRow::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_columns", "columns"),
                         &PDJE_RelationalRow::set_columns);
    ClassDB::bind_method(D_METHOD("get_columns"),
                         &PDJE_RelationalRow::get_columns);
    ClassDB::bind_method(D_METHOD("set_values", "values"),
                         &PDJE_RelationalRow::set_values);
    ClassDB::bind_method(D_METHOD("get_values"),
                         &PDJE_RelationalRow::get_values);

    ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "columns"),
                 "set_columns",
                 "get_columns");
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "values"),
                 "set_values",
                 "get_values");
}

void
PDJE_RelationalRow::set_columns(const PackedStringArray &columns)
{
    columns_ = columns;
}

PackedStringArray
PDJE_RelationalRow::get_columns() const
{
    return columns_;
}

void
PDJE_RelationalRow::set_values(const Array &values)
{
    values_ = values;
}

Array
PDJE_RelationalRow::get_values() const
{
    return values_;
}

void
PDJE_RelationalExecResult::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("set_affected_rows", "affected_rows"),
                         &PDJE_RelationalExecResult::set_affected_rows);
    ClassDB::bind_method(D_METHOD("get_affected_rows"),
                         &PDJE_RelationalExecResult::get_affected_rows);
    ClassDB::bind_method(
        D_METHOD("set_has_last_insert_rowid", "has_last_insert_rowid"),
        &PDJE_RelationalExecResult::set_has_last_insert_rowid);
    ClassDB::bind_method(D_METHOD("get_has_last_insert_rowid"),
                         &PDJE_RelationalExecResult::get_has_last_insert_rowid);
    ClassDB::bind_method(D_METHOD("set_last_insert_rowid", "last_insert_rowid"),
                         &PDJE_RelationalExecResult::set_last_insert_rowid);
    ClassDB::bind_method(D_METHOD("get_last_insert_rowid"),
                         &PDJE_RelationalExecResult::get_last_insert_rowid);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "affected_rows"),
                 "set_affected_rows",
                 "get_affected_rows");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "has_last_insert_rowid"),
                 "set_has_last_insert_rowid",
                 "get_has_last_insert_rowid");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "last_insert_rowid"),
                 "set_last_insert_rowid",
                 "get_last_insert_rowid");
}

void
PDJE_RelationalExecResult::set_affected_rows(int64_t affected_rows)
{
    affected_rows_ = affected_rows;
}

int64_t
PDJE_RelationalExecResult::get_affected_rows() const
{
    return affected_rows_;
}

void
PDJE_RelationalExecResult::set_has_last_insert_rowid(
    bool has_last_insert_rowid)
{
    has_last_insert_rowid_ = has_last_insert_rowid;
}

bool
PDJE_RelationalExecResult::get_has_last_insert_rowid() const
{
    return has_last_insert_rowid_;
}

void
PDJE_RelationalExecResult::set_last_insert_rowid(int64_t last_insert_rowid)
{
    last_insert_rowid_ = last_insert_rowid;
}

int64_t
PDJE_RelationalExecResult::get_last_insert_rowid() const
{
    return last_insert_rowid_;
}
