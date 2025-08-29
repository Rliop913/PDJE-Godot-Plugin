#include "register_types.h"

#include "PDJE_Wrapper.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void
initialize_example_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    ClassDB::register_class<EnumWrapper>();
    ClassDB::register_class<PDJE_EDITOR_ARG>();
    ClassDB::register_class<EditorWrapper>();
    ClassDB::register_class<MusPannelWrapper>();
    ClassDB::register_class<FXArgWrapper>();
    ClassDB::register_class<FXWrapper>();
    ClassDB::register_class<CoreLine>();
    

    ClassDB::register_class<PlayerWrapper>();
    ClassDB::register_class<PDJE_Wrapper>();

    // GDREGISTER_RUNTIME_CLASS(PlayerWrapper);

    // GDREGISTER_RUNTIME_CLASS(PDJE_Wrapper);
}

void
uninitialize_example_module(ModuleInitializationLevel p_level)
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT
pdje_wrapper_init(GDExtensionInterfaceGetProcAddress p_get_proc_address,
                  const GDExtensionClassLibraryPtr   p_library,
                  GDExtensionInitialization         *r_initialization)
{
    godot::GDExtensionBinding::InitObject init_obj(
        p_get_proc_address, p_library, r_initialization);

    init_obj.register_initializer(initialize_example_module);
    init_obj.register_terminator(uninitialize_example_module);
    init_obj.set_minimum_library_initialization_level(
        MODULE_INITIALIZATION_LEVEL_SCENE);

    return init_obj.init();
}
}