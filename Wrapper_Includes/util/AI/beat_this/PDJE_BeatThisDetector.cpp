#include "util/AI/beat_this/PDJE_BeatThisDetector.hpp"

#include "PDJE_Core_Wrapper.hpp"
#include "global/pdje_util_common.hpp"
#include "util/AI/beat_this/PDJE_BeatThisResult.hpp"
#include "util/ai/beat_this/BeatThis.hpp"
#include "util/common/bridge/PublicUtilBridge.hpp"

#include <algorithm>
#include <cstring>
#include <exception>
#include <filesystem>
#include <span>
#include <string>
#include <vector>

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

namespace {

using godot::pdje_public_util::common::print_method_error;

constexpr int kPdjeDecodedSampleRate = 48000;

PackedFloat64Array
ToPackedFloat64Array(const std::vector<double> &values)
{
    PackedFloat64Array packed;
    if (values.empty()) {
        return packed;
    }

    packed.resize(static_cast<int64_t>(values.size()));
    std::memcpy(packed.ptrw(), values.data(), values.size() * sizeof(double));
    return packed;
}

Ref<PDJE_BeatThisResult>
ToGodotResult(const PDJE_UTIL::ai::BeatDetectionResult &native_result)
{
    Ref<PDJE_BeatThisResult> result;
    result.instantiate();
    result->set_beats(ToPackedFloat64Array(native_result.beats));
    result->set_downbeats(ToPackedFloat64Array(native_result.downbeats));
    return result;
}

Ref<PDJE_BeatThisResult>
NullResult()
{
    return Ref<PDJE_BeatThisResult>();
}

String
ExceptionDetail(const std::exception &ex)
{
    return CStrToGStr(std::string(ex.what()));
}

std::vector<float>
DownmixInterleavedToMono(const PackedFloat32Array &pcm, const int channel_count)
{
    const int64_t frame_count = pcm.size() / channel_count;
    std::vector<float> mono(static_cast<std::size_t>(frame_count));

    for (int64_t frame = 0; frame < frame_count; ++frame) {
        double sum = 0.0;
        const int64_t base = frame * channel_count;
        for (int channel = 0; channel < channel_count; ++channel) {
            sum += static_cast<float>(pcm[base + channel]);
        }
        mono[static_cast<std::size_t>(frame)] =
            static_cast<float>(sum / static_cast<double>(channel_count));
    }

    return mono;
}

} // namespace

void
PDJE_BeatThisDetector::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("Init", "model_path"),
                         &PDJE_BeatThisDetector::Init);
    ClassDB::bind_method(D_METHOD("IsInitialized"),
                         &PDJE_BeatThisDetector::IsInitialized);
    ClassDB::bind_method(D_METHOD("GetModelPath"),
                         &PDJE_BeatThisDetector::GetModelPath);
    ClassDB::bind_method(
        D_METHOD("DetectPCM", "pcm", "channel_count", "sample_rate"),
        &PDJE_BeatThisDetector::DetectPCM);
    ClassDB::bind_method(D_METHOD("DetectMusic",
                                  "core_api",
                                  "music_title",
                                  "composer",
                                  "bpm"),
                         &PDJE_BeatThisDetector::DetectMusic,
                         DEFVAL(-1.0));
}

PDJE_BeatThisDetector::PDJE_BeatThisDetector() = default;

PDJE_BeatThisDetector::~PDJE_BeatThisDetector() = default;

bool
PDJE_BeatThisDetector::Init(String model_path)
{
    detector_.reset();

    try {
        if (model_path.is_empty()) {
            print_method_error("PDJE_BeatThisDetector.Init",
                               "'model_path' must not be empty");
            return false;
        }

        const std::filesystem::path resolved_model_path =
            GpathToCPath(model_path);
        std::error_code fs_error;

        if (!std::filesystem::exists(resolved_model_path, fs_error) ||
            fs_error) {
            print_method_error("PDJE_BeatThisDetector.Init",
                               "model file does not exist");
            return false;
        }
        if (!std::filesystem::is_regular_file(resolved_model_path,
                                              fs_error) ||
            fs_error) {
            print_method_error("PDJE_BeatThisDetector.Init",
                               "model path is not a regular file");
            return false;
        }
        if (resolved_model_path.extension() != ".onnx") {
            print_method_error("PDJE_BeatThisDetector.Init",
                               "model file must have .onnx extension");
            return false;
        }

        detector_ = std::make_unique<PDJE_UTIL::ai::BeatThisDetector>(
            resolved_model_path);
    } catch (const std::exception &ex) {
        print_method_error("PDJE_BeatThisDetector.Init",
                           ExceptionDetail(ex));
        return false;
    } catch (...) {
        print_method_error("PDJE_BeatThisDetector.Init",
                           "unknown model initialization error");
        return false;
    }

    return detector_ != nullptr;
}

bool
PDJE_BeatThisDetector::IsInitialized() const
{
    return detector_ != nullptr;
}

String
PDJE_BeatThisDetector::GetModelPath() const
{
    if (!detector_) {
        return {};
    }

    return CStrToGStr(detector_->model_path.string());
}

Ref<PDJE_BeatThisResult>
PDJE_BeatThisDetector::DetectPCM(PackedFloat32Array pcm,
                                 int                channel_count,
                                 int                sample_rate)
{
    if (!detector_) {
        print_method_error("PDJE_BeatThisDetector.DetectPCM",
                           "detector is not initialized");
        return NullResult();
    }
    if (pcm.is_empty()) {
        print_method_error("PDJE_BeatThisDetector.DetectPCM",
                           "'pcm' must not be empty");
        return NullResult();
    }
    if (channel_count < 1) {
        print_method_error("PDJE_BeatThisDetector.DetectPCM",
                           "invalid channel_count");
        return NullResult();
    }
    if (sample_rate < 1) {
        print_method_error("PDJE_BeatThisDetector.DetectPCM",
                           "invalid sample_rate");
        return NullResult();
    }
    if ((pcm.size() % channel_count) != 0) {
        print_method_error("PDJE_BeatThisDetector.DetectPCM",
                           "'pcm' length must be divisible by channel_count");
        return NullResult();
    }

    try {
        std::vector<float> mono = DownmixInterleavedToMono(pcm, channel_count);
        const std::span<const float> samples(mono.data(), mono.size());
        return ToGodotResult(detector_->detect(samples, sample_rate));
    } catch (const std::exception &ex) {
        print_method_error("PDJE_BeatThisDetector.DetectPCM",
                           ExceptionDetail(ex));
    } catch (...) {
        print_method_error("PDJE_BeatThisDetector.DetectPCM",
                           "unknown detection error");
    }

    return NullResult();
}

Ref<PDJE_BeatThisResult>
PDJE_BeatThisDetector::DetectMusic(PDJE_Wrapper *core_api,
                                   String        music_title,
                                   String        composer,
                                   double        bpm)
{
    if (!detector_) {
        print_method_error("PDJE_BeatThisDetector.DetectMusic",
                           "detector is not initialized");
        return NullResult();
    }
    if (core_api == nullptr) {
        print_method_error("PDJE_BeatThisDetector.DetectMusic",
                           "core_api is null");
        return NullResult();
    }
    if (!core_api->engine.has_value()) {
        print_method_error("PDJE_BeatThisDetector.DetectMusic",
                           "core_api engine is not initialized");
        return NullResult();
    }

    try {
        auto mus_searched = core_api->engine->SearchMusic(
            GStrToCStr(music_title), GStrToCStr(composer), bpm);
        if (mus_searched.empty()) {
            print_method_error("PDJE_BeatThisDetector.DetectMusic",
                               "music data not found");
            return NullResult();
        }

        auto pcm = core_api->engine->GetPCMFromMusData(mus_searched.front(), 1);
        if (pcm.empty()) {
            print_method_error("PDJE_BeatThisDetector.DetectMusic",
                               "PDJE returned empty PCM");
            return NullResult();
        }

        const std::span<const float> samples(pcm.data(), pcm.size());
        return ToGodotResult(
            detector_->detect(samples, kPdjeDecodedSampleRate));
    } catch (const std::exception &ex) {
        print_method_error("PDJE_BeatThisDetector.DetectMusic",
                           ExceptionDetail(ex));
    } catch (...) {
        print_method_error("PDJE_BeatThisDetector.DetectMusic",
                           "unknown detection error");
    }

    return NullResult();
}
