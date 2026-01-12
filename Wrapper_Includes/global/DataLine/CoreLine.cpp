#include "CoreLine.hpp"
#include "PDJE_Utils.hpp"
#include "variant/array.hpp"
#include "variant/packed_float32_array.hpp"
#include "variant/packed_int64_array.hpp"
#include <atomic>

using namespace godot;

void
CoreLine::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("GetUsedFrame"), &CoreLine::GetEngineTime);
    ClassDB::bind_method(D_METHOD("GetNowCursor"), &CoreLine::GetNowCursor);
    ClassDB::bind_method(D_METHOD("GetMaxCursor"), &CoreLine::GetMaxCursor);
    ClassDB::bind_method(D_METHOD("GetPreRenderedFrames"),
                         &CoreLine::GetPreRenderedFrames);
}

void
CoreLine::Init(const PDJE_CORE_DATA_LINE &coreDataLine)
{

    core_data = coreDataLine;
}

PackedInt64Array
CoreLine::GetEngineTime()
{

    auto             synced = core_data.syncD->load(std::memory_order_acquire);
    PackedInt64Array out;

    out.push_back(synced.consumed_frames);
    out.push_back(synced.microsecond);
    return out;
}

int64_t
CoreLine::GetNowCursor()
{

    return core_data.nowCursor ? static_cast<int64_t>(*(core_data.nowCursor))
                               : 0;
}

int64_t
CoreLine::GetMaxCursor()
{

    return core_data.maxCursor ? static_cast<int64_t>(*(core_data.maxCursor))
                               : 0;
}

#define PDJE_STEREO_CHANNEL 2

PackedFloat32Array
CoreLine::GetPreRenderedFrames()
{

    PackedFloat32Array out;

    if (!core_data.maxCursor || !core_data.preRenderedData) {
        WARN_PRINT("GetPreRenderedFrames: null pointer (maxCursor or "
                   "preRenderedData).");
        return out; // empty
    }

    unsigned long long frames = *core_data.maxCursor;
    if (frames == 0) {
        return out;
    }

    unsigned long long totalFrames = frames * (uint64_t)PDJE_STEREO_CHANNEL;

    if (totalFrames > (uint64_t)std::numeric_limits<int64_t>::max()) {
        ERR_PRINT("GetPreRenderedFrames: total_samples overflow.");
        return out;
    }

    int64_t total_samples = static_cast<int64_t>(totalFrames);
    out.resize(total_samples);
    float *dst = out.ptrw();

    std::memcpy(dst,
                core_data.preRenderedData,
                static_cast<size_t>(total_samples) * sizeof(float));
    return out;
}

#undef PDJE_STEREO_CHANNEL