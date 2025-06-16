set(cmake_cxx_standard 20)
set(cmake_cxx_flags -std=c++20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_definitions(TYPED_METHOD_BIND)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_minimum_required(VERSION 3.10.0)
set(HWY_ENABLE_TESTS OFF)
set(CAPNP_BUILD_TESTS OFF)
set(BUILD_TESTING OFF CACHE BOOL "Disable testing")
set(HWY_ENABLE_TARGETS "scalar,sse4,avx,avx2" CACHE STRING "Enabled SIMD targets" FORCE)

if(WIN32)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "MSVC runtime" FORCE)
endif()

project(PDJE_godot_wrapper VERSION 0.1.0 LANGUAGES C CXX)


include(ExternalProject)
set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-${CMAKE_BUILD_TYPE}")

string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM_ID_LOWER)

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
include(cmake/godotExternalSetter.cmake)

# include(cmake/pdjeExternalSetter.cmake)



add_subdirectory(${CMAKE_SOURCE_DIR}/Project_DJ_Engine)


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

# target_compile_options(PDJE PUBLIC -fexceptions -funwind-tables)
target_link_libraries(PDJE_godot_wrapper PUBLIC PDJE_dynamic godotcppEx
# ${CMAKE_BINARY_DIR}/bin/libgodot-cpp.linux.template_debug.x86_64.a
)

set_target_properties(PDJE_godot_wrapper PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/PDJE_Prebuilt/Release"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/PDJE_Prebuilt/Debug"
)
if(WIN32)

target_compile_definitions(PDJE_godot_wrapper PUBLIC PDJE_WINDOWS_DLL)
endif()
message(${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/gen/include)
target_include_directories( PDJE_godot_wrapper PRIVATE
    ${CMAKE_SOURCE_DIR}/Wrapper_src
    ${CMAKE_SOURCE_DIR}/Wrapper_src/Player
    ${CMAKE_SOURCE_DIR}/Wrapper_src/Editor
    
    ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/gen/include
    ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/gen/include/godot_cpp
    ${CMAKE_SOURCE_DIR}/godot-cpp/include
    ${CMAKE_SOURCE_DIR}/godot-cpp/include/godot_cpp
    ${CMAKE_SOURCE_DIR}/godot-cpp/gdextension
    
    
    
  
)
