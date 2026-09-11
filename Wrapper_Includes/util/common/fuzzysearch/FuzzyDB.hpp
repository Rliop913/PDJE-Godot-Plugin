#pragma once

#include "util/function/fuzzy/FuzzySearch.hpp"

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class FuzzyDB : public RefCounted {
    GDCLASS(FuzzyDB, RefCounted)

  private:
    PDJE_UTIL::function::fuzzy::FuzzySearch fuzzy_search_;

  protected:
    static void
    _bind_methods();

  public:
    void
    store(String value);

    Array
    query(String value, int max_candidates) const;
};

} // namespace godot
