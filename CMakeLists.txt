set(cmake_cxx_standard 20)
set(cmake_cxx_flags -std=c++20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.10.0)
set(HWY_ENABLE_TESTS OFF)
set(CAPNP_BUILD_TESTS OFF)
set(BUILD_TESTING OFF CACHE BOOL "Disable testing")
set(HWY_ENABLE_TARGETS "scalar,sse4,avx,avx2" CACHE STRING "Enabled SIMD targets" FORCE)

project(PDJE_godot_wrapper VERSION 0.1.0 LANGUAGES C CXX)


add_subdirectory(${CMAKE_SOURCE_DIR}/godot-cpp)
add_subdirectory(${CMAKE_SOURCE_DIR}/Project_DJ_Engine)
# include(ExternalProject)

# ExternalProject_Add(PDJE
#     SOURCE_DIR ${CMAKE_SOURCE_DIR}/Project_DJ_Engine
#     CMAKE_ARGS
#         -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/pdje
#         -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#     BUILD_COMMAND ${CMAKE_COMMAND} --build . --parallel 12
#     INSTALL_DIR ${CMAKE_BINARY_DIR}/_install/pdje
# )
# add_subdirectory(${CMAKE_SOURCE_DIR}/Project_DJ_Engine)



add_library(PDJE_godot_wrapper SHARED 
${CMAKE_SOURCE_DIR}/Wrapper_src/Player/FXArgWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_src/PDJE_Wrapper.cpp 
${CMAKE_SOURCE_DIR}/Wrapper_src/register_types.cpp
${CMAKE_SOURCE_DIR}/Wrapper_src/Player/PlayerWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_src/Player/FXWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_src/Player/EnumWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_src/Player/MusPannelWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_src/Editor/EditorWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_src/Editor/EditorArgs.cpp
${CMAKE_SOURCE_DIR}/Wrapper_src/Editor/EditorFunctions.cpp





)
target_link_libraries(PDJE_godot_wrapper PRIVATE PDJE 
${CMAKE_BINARY_DIR}/bin/libgodot-cpp.linux.template_debug.x86_64.a
)

set_target_properties(PDJE_godot_wrapper PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/PDJE_Prebuilt"
)

message(${CMAKE_BINARY_DIR})
target_include_directories( PDJE_godot_wrapper PRIVATE
    ${CMAKE_SOURCE_DIR}/Wrapper_src
    ${CMAKE_SOURCE_DIR}/Wrapper_src/Player
    ${CMAKE_SOURCE_DIR}/Wrapper_src/Editor
    
    ${CMAKE_BINARY_DIR}/godot-cpp/gen/include
    ${CMAKE_BINARY_DIR}/godot-cpp/gen/include/godot_cpp
    ${CMAKE_SOURCE_DIR}/godot-cpp/include
    ${CMAKE_SOURCE_DIR}/godot-cpp/include/godot_cpp
    ${CMAKE_SOURCE_DIR}/godot-cpp/gdextension
    
    
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/db
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender/MixMachine
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender/MixMachine/EFFECT
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender/MixMachine/FAUST_VALS
    
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender/MixMachine/MUSIC_CTR
    
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender/MixMachine/MiniaudioObjects
    
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender/MixMachine/INTERPOLATOR
  
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender/ManualMix
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/audioRender/ManualMix/ManualFausts
    
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/db/Capnp/CapnpBinary
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/db/Capnp/Translators
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/db/Capnp/Translators/MixTranslator
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/db/Capnp/Translators/MusicTranslator
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/db/Capnp/Translators/NoteTranslator
  
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/third_party/Capnp
    
    #   ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/third_party/SoundTouch/soundtouch_include
    #   ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/third_party/Faust/compiled
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/third_party/Faust/manual_compiled
    
  
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/input
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/input/linux
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/input/macos
    
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/MainObjects
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/MainObjects/audioPlayer
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/MainObjects/editorObject
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/MainObjects/tempDBObject
    
  
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/interface
  
  
  
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/edit
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper/GitWrap
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper/GitWrap/Branch
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper/GitWrap/Commit
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper/GitWrap/Add
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper/GitWrap/Blame
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper/GitWrap/Diff
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper/GitWrap/Log
    
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/featureWrapper/JSONWrap
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/objects
    # ${CMAKE_SOURCE_DIR}/Project_DJ_Engine/include/editor/renderer
    
    
    
  
  
  
)
