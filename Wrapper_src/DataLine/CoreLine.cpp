#include "CoreLine.hpp"

using namespace godot;

void
CoreLine::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("GetUsedFrame"), &CoreLine::GetUsedFrame);
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

int64_t
CoreLine::GetUsedFrame()
{

    return core_data.used_frame ? static_cast<int64_t>(*(core_data.used_frame))
                                : 0;
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