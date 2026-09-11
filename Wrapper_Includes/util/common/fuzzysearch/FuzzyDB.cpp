#include "util/common/fuzzysearch/FuzzyDB.hpp"

#include "global/pdje_util_common.hpp"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/dictionary.hpp>

namespace godot {

void
FuzzyDB::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("store", "value"), &FuzzyDB::store);
    ClassDB::bind_method(D_METHOD("query", "value", "max_candidates"),
                         &FuzzyDB::query);
}

void
FuzzyDB::store(String value)
{
    fuzzy_search_.store(GStrToCStr(value));
}

Array
FuzzyDB::query(String value, int max_candidates) const
{
    Array results;
    for (const auto &[similarity, candidate] :
         fuzzy_search_.query(GStrToCStr(value), max_candidates)) {
        Dictionary result;
        result["similarity"]     = static_cast<double>(similarity);
        result["queried_string"] = CStrToGStr(candidate);
        results.push_back(result);
    }
    return results;
}

} // namespace godot
