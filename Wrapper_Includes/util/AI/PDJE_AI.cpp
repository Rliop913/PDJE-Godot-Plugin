#include "util/AI/PDJE_AI.hpp"

#include "util/AI/beat_this/PDJE_BeatThisDetector.hpp"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void
PDJE_AI::_bind_methods()
{
    ClassDB::bind_method(D_METHOD("CreateBeatThisDetector", "model_path"),
                         &PDJE_AI::CreateBeatThisDetector,
                         DEFVAL(""));
}

PDJE_AI::PDJE_AI() = default;

PDJE_AI::~PDJE_AI() = default;

Ref<PDJE_BeatThisDetector>
PDJE_AI::CreateBeatThisDetector(String model_path)
{
    Ref<PDJE_BeatThisDetector> detector;
    detector.instantiate();
    if (!detector->Init(model_path)) {
        return {};
    }

    return detector;
}

void
PDJE_AI::_ready()
{
}
