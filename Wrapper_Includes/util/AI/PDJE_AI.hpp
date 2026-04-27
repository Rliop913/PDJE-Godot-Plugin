#pragma once

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/variant/string.hpp>

namespace godot {

class PDJE_BeatThisDetector;

class PDJE_AI : public Node {
    GDCLASS(PDJE_AI, Node)

  protected:
    static void
    _bind_methods();

  public:
    Ref<PDJE_BeatThisDetector>
    CreateBeatThisDetector(String model_path = "");

    PDJE_AI();
    ~PDJE_AI();

    void
    _ready() override;
};

} // namespace godot
