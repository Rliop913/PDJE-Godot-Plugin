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
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/conan_cmakes/conan_toolchain.cmake")
cmake_policy(SET CMP0091 NEW)
# if(APPLE)
# set(HWY_ENABLE_TARGETS "scalar,sse4,avx,avx2" CACHE STRING "Enabled SIMD targets" FORCE)
# endif()
# if(WIN32)
#   set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "MSVC runtime" FORCE)
# endif()

project(PDJE_godot_wrapper VERSION 0.1.0 LANGUAGES C CXX)


set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-${CMAKE_BUILD_TYPE}")

string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM_ID_LOWER)

string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)
include(ExternalProject)
include(cmake/godotExternalSetter.cmake)

# include(cmake/pdjeExternalSetter.cmake)

set(PDJE_DYNAMIC ON CACHE BOOL "Make Shared Library")
set(PDJE_SWIG_BUILD OFF CACHE BOOL "Turn off swig build")


add_subdirectory(${CMAKE_SOURCE_DIR}/Project-DJ-Engine)


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
${CMAKE_SOURCE_DIR}/Wrapper_src/DataLine/CoreLine.cpp

)

# target_compile_options(PDJE PUBLIC -fexceptions -funwind-tables)
target_link_libraries(PDJE_godot_wrapper PUBLIC PDJE godotcppEx)

add_dependencies(PDJE_godot_wrapper godotcppEx)

if(WIN32)

target_compile_definitions(PDJE_godot_wrapper PUBLIC PDJE_WINDOWS_DLL)
endif()
target_include_directories( PDJE_godot_wrapper PUBLIC
    ${CMAKE_SOURCE_DIR}/Wrapper_src
    ${CMAKE_SOURCE_DIR}/Wrapper_src/Player
    ${CMAKE_SOURCE_DIR}/Wrapper_src/Editor
    ${CMAKE_SOURCE_DIR}/Wrapper_src/DataLine
    
    
    ${binary_dir}/gen/include
    ${binary_dir}/gen/include/godot_cpp
    ${source_dir}/include
    ${source_dir}/include/godot_cpp
    ${source_dir}/gdextension 
)


if(APPLE)
set_target_properties(PDJE_godot_wrapper PROPERTIES
    MACOSX_RPATH ON
    BUILD_RPATH "@loader_path"
)
target_link_options(PDJE_godot_wrapper PRIVATE
    "-Wl,-rpath,@loader_path"
)

elseif(UNIX)
set_target_properties(PDJE_godot_wrapper PROPERTIES
    BUILD_RPATH "\$ORIGIN"
)
target_link_options(PDJE_godot_wrapper PRIVATE
"-Wl,--enable-new-dtags"
"-Wl,-rpath,\$ORIGIN"
)

endif()


# get_cmake_property(_vars VARIABLES)

# foreach(var ${_vars})
#     if(var MATCHES "^PDJE")
#         message(STATUS "환경변수: ${var} = [${${var}}]")
#     endif()
# endforeach()
