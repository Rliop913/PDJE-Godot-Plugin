#pragma once
#include <godot_cpp/classes/node.hpp>
#include "PDJE_interface.hpp"
// #include "PDJE_interface.hpp"

namespace godot {

class PDJE_Wrapper : public Node {
	GDCLASS(PDJE_Wrapper, Node)

private:
	std::optional<PDJE> engine;

protected:
	static void _bind_methods();

public:
	PDJE_Wrapper();
	~PDJE_Wrapper();

	void _ready() override;
	void _process(double delta) override;
};

}
