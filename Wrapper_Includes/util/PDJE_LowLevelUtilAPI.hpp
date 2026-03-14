#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/variant.hpp>

#include <memory>

namespace godot {

namespace pdje_low_level_util_internal {
struct KeyValueState;
struct NearestState;
struct RelationalState;
} // namespace pdje_low_level_util_internal

class PDJE_LowLevelUtilAPI : public Node {
    GDCLASS(PDJE_LowLevelUtilAPI, Node)

  private:
    std::unique_ptr<pdje_low_level_util_internal::KeyValueState>
                                                                keyvalue_state_;
    std::unique_ptr<pdje_low_level_util_internal::NearestState> nearest_state_;
    std::unique_ptr<pdje_low_level_util_internal::RelationalState>
        relational_state_;

  protected:
    static void
    _bind_methods();

  public:
    Dictionary
    KeyValueCreate(Dictionary config);

    Dictionary
    KeyValueDestroy(Dictionary config);

    Dictionary
    KeyValueOpen(Dictionary config);

    Dictionary
    KeyValueClose();

    Dictionary
    KeyValueContains(String key);

    Dictionary
    KeyValueGetText(String key);

    Dictionary
    KeyValueGetBytes(String key);

    Dictionary
    KeyValuePutText(String key, String value);

    Dictionary
    KeyValuePutBytes(String key, PackedByteArray value);

    Dictionary
    KeyValueErase(String key);

    Dictionary
    KeyValueListKeys(String prefix = "");

    Dictionary
    NearestCreate(Dictionary config);

    Dictionary
    NearestDestroy(Dictionary config);

    Dictionary
    NearestOpen(Dictionary config);

    Dictionary
    NearestClose();

    Dictionary
    NearestContains(String id);

    Dictionary
    NearestGetItem(String id);

    Dictionary
    NearestUpsertItem(Dictionary item);

    Dictionary
    NearestEraseItem(String id);

    Dictionary
    NearestSearch(Array query_embedding, Dictionary options = Dictionary());

    Dictionary
    NearestListKeys();

    Dictionary
    RelationalCreate(Dictionary config);

    Dictionary
    RelationalDestroy(Dictionary config);

    Dictionary
    RelationalOpen(Dictionary config);

    Dictionary
    RelationalClose();

    Dictionary
    RelationalExecute(String sql, Array params = Array());

    Dictionary
    RelationalQuery(String sql, Array params = Array());

    Dictionary
    RelationalBeginTransaction();

    Dictionary
    RelationalCommit();

    Dictionary
    RelationalRollback();

    Dictionary
    EncodeWaveformWebps(Array pcm,
                        int   channel_count,
                        int   y_pixels,
                        int   pcm_per_pixel,
                        int   x_pixels_per_image,
                        int   compression_level   = -1,
                        int   worker_thread_count = 0);

    PDJE_LowLevelUtilAPI();
    ~PDJE_LowLevelUtilAPI();

    void
    _ready() override;
};

} // namespace godot
