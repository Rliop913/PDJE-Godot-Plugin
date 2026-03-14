#include "PDJE_LowLevelUtilAPI.hpp"

#include "util/db/keyvalue/adapter/LowLevelKeyValueAdapter.hpp"
#include "util/db/nearest/adapter/LowLevelNearestAdapter.hpp"
#include "util/db/relational/adapter/LowLevelRelationalAdapter.hpp"
#include "util/function/image/waveform/adapter/LowLevelWaveformAdapter.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void
PDJE_LowLevelUtilAPI::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("KeyValueCreate", "config"),
                         &PDJE_LowLevelUtilAPI::KeyValueCreate);
    ClassDB::bind_method(D_METHOD("KeyValueDestroy", "config"),
                         &PDJE_LowLevelUtilAPI::KeyValueDestroy);
    ClassDB::bind_method(D_METHOD("KeyValueOpen", "config"),
                         &PDJE_LowLevelUtilAPI::KeyValueOpen);
    ClassDB::bind_method(D_METHOD("KeyValueClose"),
                         &PDJE_LowLevelUtilAPI::KeyValueClose);
    ClassDB::bind_method(D_METHOD("KeyValueContains", "key"),
                         &PDJE_LowLevelUtilAPI::KeyValueContains);
    ClassDB::bind_method(D_METHOD("KeyValueGetText", "key"),
                         &PDJE_LowLevelUtilAPI::KeyValueGetText);
    ClassDB::bind_method(D_METHOD("KeyValueGetBytes", "key"),
                         &PDJE_LowLevelUtilAPI::KeyValueGetBytes);
    ClassDB::bind_method(D_METHOD("KeyValuePutText", "key", "value"),
                         &PDJE_LowLevelUtilAPI::KeyValuePutText);
    ClassDB::bind_method(D_METHOD("KeyValuePutBytes", "key", "value"),
                         &PDJE_LowLevelUtilAPI::KeyValuePutBytes);
    ClassDB::bind_method(D_METHOD("KeyValueErase", "key"),
                         &PDJE_LowLevelUtilAPI::KeyValueErase);
    ClassDB::bind_method(D_METHOD("KeyValueListKeys", "prefix"),
                         &PDJE_LowLevelUtilAPI::KeyValueListKeys,
                         DEFVAL(""));

    ClassDB::bind_method(D_METHOD("NearestCreate", "config"),
                         &PDJE_LowLevelUtilAPI::NearestCreate);
    ClassDB::bind_method(D_METHOD("NearestDestroy", "config"),
                         &PDJE_LowLevelUtilAPI::NearestDestroy);
    ClassDB::bind_method(D_METHOD("NearestOpen", "config"),
                         &PDJE_LowLevelUtilAPI::NearestOpen);
    ClassDB::bind_method(D_METHOD("NearestClose"),
                         &PDJE_LowLevelUtilAPI::NearestClose);
    ClassDB::bind_method(D_METHOD("NearestContains", "id"),
                         &PDJE_LowLevelUtilAPI::NearestContains);
    ClassDB::bind_method(D_METHOD("NearestGetItem", "id"),
                         &PDJE_LowLevelUtilAPI::NearestGetItem);
    ClassDB::bind_method(D_METHOD("NearestUpsertItem", "item"),
                         &PDJE_LowLevelUtilAPI::NearestUpsertItem);
    ClassDB::bind_method(D_METHOD("NearestEraseItem", "id"),
                         &PDJE_LowLevelUtilAPI::NearestEraseItem);
    ClassDB::bind_method(
        D_METHOD("NearestSearch", "query_embedding", "options"),
        &PDJE_LowLevelUtilAPI::NearestSearch,
        DEFVAL(Dictionary()));
    ClassDB::bind_method(D_METHOD("NearestListKeys"),
                         &PDJE_LowLevelUtilAPI::NearestListKeys);

    ClassDB::bind_method(D_METHOD("RelationalCreate", "config"),
                         &PDJE_LowLevelUtilAPI::RelationalCreate);
    ClassDB::bind_method(D_METHOD("RelationalDestroy", "config"),
                         &PDJE_LowLevelUtilAPI::RelationalDestroy);
    ClassDB::bind_method(D_METHOD("RelationalOpen", "config"),
                         &PDJE_LowLevelUtilAPI::RelationalOpen);
    ClassDB::bind_method(D_METHOD("RelationalClose"),
                         &PDJE_LowLevelUtilAPI::RelationalClose);
    ClassDB::bind_method(D_METHOD("RelationalExecute", "sql", "params"),
                         &PDJE_LowLevelUtilAPI::RelationalExecute,
                         DEFVAL(Array()));
    ClassDB::bind_method(D_METHOD("RelationalQuery", "sql", "params"),
                         &PDJE_LowLevelUtilAPI::RelationalQuery,
                         DEFVAL(Array()));
    ClassDB::bind_method(D_METHOD("RelationalBeginTransaction"),
                         &PDJE_LowLevelUtilAPI::RelationalBeginTransaction);
    ClassDB::bind_method(D_METHOD("RelationalCommit"),
                         &PDJE_LowLevelUtilAPI::RelationalCommit);
    ClassDB::bind_method(D_METHOD("RelationalRollback"),
                         &PDJE_LowLevelUtilAPI::RelationalRollback);

    ClassDB::bind_method(D_METHOD("EncodeWaveformWebps",
                                  "pcm",
                                  "channel_count",
                                  "y_pixels",
                                  "pcm_per_pixel",
                                  "x_pixels_per_image",
                                  "compression_level",
                                  "worker_thread_count"),
                         &PDJE_LowLevelUtilAPI::EncodeWaveformWebps,
                         DEFVAL(-1),
                         DEFVAL(0));
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueCreate(Dictionary config)
{
    return pdje_low_level_util::keyvalue::Create(keyvalue_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueDestroy(Dictionary config)
{
    return pdje_low_level_util::keyvalue::Destroy(keyvalue_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueOpen(Dictionary config)
{
    return pdje_low_level_util::keyvalue::Open(keyvalue_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueClose()
{
    return pdje_low_level_util::keyvalue::Close(keyvalue_state_);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueContains(String key)
{
    return pdje_low_level_util::keyvalue::Contains(keyvalue_state_, key);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueGetText(String key)
{
    return pdje_low_level_util::keyvalue::GetText(keyvalue_state_, key);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueGetBytes(String key)
{
    return pdje_low_level_util::keyvalue::GetBytes(keyvalue_state_, key);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValuePutText(String key, String value)
{
    return pdje_low_level_util::keyvalue::PutText(keyvalue_state_, key, value);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValuePutBytes(String key, PackedByteArray value)
{
    return pdje_low_level_util::keyvalue::PutBytes(keyvalue_state_, key, value);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueErase(String key)
{
    return pdje_low_level_util::keyvalue::Erase(keyvalue_state_, key);
}

Dictionary
PDJE_LowLevelUtilAPI::KeyValueListKeys(String prefix)
{
    return pdje_low_level_util::keyvalue::ListKeys(keyvalue_state_, prefix);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestCreate(Dictionary config)
{
    return pdje_low_level_util::nearest::Create(nearest_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestDestroy(Dictionary config)
{
    return pdje_low_level_util::nearest::Destroy(nearest_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestOpen(Dictionary config)
{
    return pdje_low_level_util::nearest::Open(nearest_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestClose()
{
    return pdje_low_level_util::nearest::Close(nearest_state_);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestContains(String id)
{
    return pdje_low_level_util::nearest::Contains(nearest_state_, id);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestGetItem(String id)
{
    return pdje_low_level_util::nearest::GetItem(nearest_state_, id);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestUpsertItem(Dictionary item)
{
    return pdje_low_level_util::nearest::UpsertItem(nearest_state_, item);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestEraseItem(String id)
{
    return pdje_low_level_util::nearest::EraseItem(nearest_state_, id);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestSearch(Array query_embedding, Dictionary options)
{
    return pdje_low_level_util::nearest::Search(
        nearest_state_, query_embedding, options);
}

Dictionary
PDJE_LowLevelUtilAPI::NearestListKeys()
{
    return pdje_low_level_util::nearest::ListKeys(nearest_state_);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalCreate(Dictionary config)
{
    return pdje_low_level_util::relational::Create(relational_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalDestroy(Dictionary config)
{
    return pdje_low_level_util::relational::Destroy(relational_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalOpen(Dictionary config)
{
    return pdje_low_level_util::relational::Open(relational_state_, config);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalClose()
{
    return pdje_low_level_util::relational::Close(relational_state_);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalExecute(String sql, Array params)
{
    return pdje_low_level_util::relational::Execute(
        relational_state_, sql, params);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalQuery(String sql, Array params)
{
    return pdje_low_level_util::relational::Query(
        relational_state_, sql, params);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalBeginTransaction()
{
    return pdje_low_level_util::relational::BeginTransaction(relational_state_);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalCommit()
{
    return pdje_low_level_util::relational::Commit(relational_state_);
}

Dictionary
PDJE_LowLevelUtilAPI::RelationalRollback()
{
    return pdje_low_level_util::relational::Rollback(relational_state_);
}

Dictionary
PDJE_LowLevelUtilAPI::EncodeWaveformWebps(Array pcm,
                                          int   channel_count,
                                          int   y_pixels,
                                          int   pcm_per_pixel,
                                          int   x_pixels_per_image,
                                          int   compression_level,
                                          int   worker_thread_count)
{
    return pdje_low_level_util::waveform::EncodeWaveformWebps(
        pcm,
        channel_count,
        y_pixels,
        pcm_per_pixel,
        x_pixels_per_image,
        compression_level,
        worker_thread_count);
}

PDJE_LowLevelUtilAPI::PDJE_LowLevelUtilAPI() = default;

PDJE_LowLevelUtilAPI::~PDJE_LowLevelUtilAPI()
{
    pdje_low_level_util::keyvalue::Shutdown(keyvalue_state_);
    pdje_low_level_util::nearest::Shutdown(nearest_state_);
    pdje_low_level_util::relational::Shutdown(relational_state_);
}

void
PDJE_LowLevelUtilAPI::_ready()
{
}
