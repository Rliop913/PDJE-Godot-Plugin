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
set(LIBREMIDI_HEADER_ONLY OFF CACHE BOOL "libremidi header only")
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
set(PDJE_INCLUDE_ROOT ${CMAKE_SOURCE_DIR}/Project-DJ-Engine CACHE STRING "SET pdje root")
message(${PDJE_INCLUDE_ROOT})
set(PDJE_DYNAMIC ON CACHE BOOL "Make Shared Library")
set(PDJE_SWIG_BUILD OFF CACHE BOOL "Turn off swig build")
set(PDJE_TEST OFF CACHE BOOL "Disable PDJE unit tests in wrapper build" FORCE)
set(PDJE_DEV_TEST OFF CACHE BOOL "Disable PDJE dev/manual tests in wrapper build" FORCE)
set(PDJE_RTTI_FORCE_OFF OFF CACHE BOOL "Disable frtti" FORCE)

if(APPLE)
set(PDJE_DEVELOP_INPUT OFF CACHE BOOL "Turn off input build")
else()
set(PDJE_DEVELOP_INPUT ON CACHE BOOL "Turn on input build")
endif()
add_subdirectory(${CMAKE_SOURCE_DIR}/Project-DJ-Engine)

if(WIN32 AND PDJE_DYNAMIC AND TARGET PDJE_MODULE_INPUT_PROCESS)
# Prevent PDJE_API from resolving to dllimport inside the input subprocess EXE.
target_compile_definitions(PDJE_MODULE_INPUT_PROCESS PRIVATE PDJE_BUILDING)

endif()

set(WrapperSrcs
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Player/FXArgWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/PDJE_Core_Wrapper.cpp 
${CMAKE_SOURCE_DIR}/Wrapper_Includes/register_types.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Player/PlayerWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Player/FXWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Player/EnumWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Player/MusPanelWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Editor/EditorWrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Editor/WrappedEditorArgs.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Editor/EditorFunctions.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/global/DataLine/CoreLine.cpp
)

# Low-level util implementation lives in the wrapper layer, so the wrapper
# target owns these sources directly instead of relying on Project-DJ-Engine.
set(UtilBundleSrcs
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/MIR/PDJE_StftResult.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/MIR/PDJE_MIR_Bindings.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/MIR/PDJE_MIR_Waveform.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/MIR/PDJE_MIR_STFT.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/db/keyvalue/PDJE_KeyValueDB.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/db/nearest/PDJE_VectorDB.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/db/nearest/PDJE_VectorTypes.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/db/relational/PDJE_RelationalDB.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/db/relational/PDJE_RelationalTypes.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/util/MIR/LowLevelWaveformAdapter.cpp
)

set(PDJE_GODOT_INCLUDE_DIRS
    ${binary_dir}/gen/include
    ${binary_dir}/gen/include/godot_cpp
    ${source_dir}/include
    ${source_dir}/include/godot_cpp
    ${source_dir}/gdextension
)

set(PDJE_WRAPPER_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/Wrapper_Includes
    ${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Player
    ${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/
    ${CMAKE_SOURCE_DIR}/Wrapper_Includes/core/Editor
    ${CMAKE_SOURCE_DIR}/Wrapper_Includes/global/DataLine
    ${CMAKE_SOURCE_DIR}/Wrapper_Includes/global/
    ${CMAKE_SOURCE_DIR}/Wrapper_Includes/input/
    ${CMAKE_SOURCE_DIR}/Wrapper_Includes/judge
    ${CMAKE_SOURCE_DIR}/Project-DJ-Engine/include
    ${CMAKE_SOURCE_DIR}/Project-DJ-Engine/include/util
    # ${PDJE_INCLUDE_CORE}
    # ${PDJE_INCLUDE_GLOBAL}
    # ${PDJE_INCLUDE_INPUT_MAINPROC}
    # ${PDJE_INCLUDE_JUDGE}
    ${CMAKE_SOURCE_DIR}/conan_cmakes/direct_deploy/annoy/include
    ${PDJE_GODOT_INCLUDE_DIRS}
)

if(PDJE_DEVELOP_INPUT)
list(APPEND WrapperSrcs
${CMAKE_SOURCE_DIR}/Wrapper_Includes/input/PDJE_Input_Wrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/judge/PDJE_Judge_Wrapper.cpp
${CMAKE_SOURCE_DIR}/Wrapper_Includes/global/DataLine/InputLine.cpp

)
endif()
add_library(PDJE_godot_wrapper SHARED 
${WrapperSrcs}
${UtilBundleSrcs}
)
set_target_properties(PDJE_godot_wrapper PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(PDJE_godot_wrapper PROPERTIES VISIBILITY_INLINES_HIDDEN ON)
if(UNIX AND NOT APPLE)
  target_link_options(PDJE_godot_wrapper PRIVATE "-Wl,--exclude-libs,ALL")
  target_link_libraries(PDJE_godot_wrapper PRIVATE -static-libstdc++ -static-libgcc)
endif()



# target_compile_options(PDJE PRIVATE -fexceptions -funwind-tables)
# if(WIN32)
# add_library(PDJE_UTIL_BUNDLE 
# # ${UtilBundleSrcs}
# )

# target_link_libraries(PDJE_godot_wrapper PRIVATE PDJE_UTIL)
# target_link_libraries(PDJE_godot_wrapper PRIVATE PDJE_UTIL_IMAGE_WEBP)
# target_link_libraries(PDJE_godot_wrapper PRIVATE PDJE_UTIL_IMAGE_WAVEFORM)
# target_link_libraries(PDJE_godot_wrapper PRIVATE PDJE_UTIL_DB)
# target_link_libraries(PDJE_godot_wrapper PUBLIC PDJE_UTIL_INCLUDES)
# target_include_directories(PDJE_godot_wrapper PRIVATE 
#     ${PDJE_WRAPPER_INCLUDE_DIRS}
# )
target_compile_definitions(PDJE_godot_wrapper PRIVATE SPDLOG_FMT_EXTERNAL)
target_compile_definitions(PDJE PRIVATE SPDLOG_FMT_EXTERNAL)
# target_link_options(PDJE_godot_wrapper PRIVATE -Wl,-z,defs)

DynamicOuterFlag(PDJE_godot_wrapper)
DynamicOuterFlag(PDJE)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
target_compile_options(PDJE_godot_wrapper PRIVATE -frtti)
endif()
# target_link_options(PDJE_godot_wrapper PRIVATE  -frtti)
# target_compile_options(PDJE PRIVATE -fsanitize=address)
# target_link_options(PDJE PRIVATE -fsanitize=address )
set(_pdje_wrapper_link_libs
PDJE
CORE_INCLUDE # PDJE related includes req -- all depts
GLOBAL_INCLUDE
SOUNDTOUCH_INCLUDE
MINIAUDIO_INCLUDE
IPC_INCLUDE
CRYPTO_INCLUDE
godotcppEx
PDJE_LOG_RUNTIME
PDJE_UTIL
PDJE_UTIL_DB
PDJE_UTIL_IMAGE_WAVEFORM
# PDJE_UTIL_BUNDLE
)

if(PDJE_DEVELOP_INPUT)
list(APPEND _pdje_wrapper_link_libs
INPUT_MAIN_INCLUDE
JUDGE_INCLUDE
)
endif()

target_link_libraries(PDJE_godot_wrapper PRIVATE
${_pdje_wrapper_link_libs}
)



find_package(CapnProto CONFIG REQUIRED)# capnp req -- dept 1
setCapnpReqLib(PDJE_godot_wrapper)

find_package(libgit2 CONFIG REQUIRED)# libgit req -- dept 2
setLibgit2ReqLib(PDJE_godot_wrapper)

find_package(botan CONFIG REQUIRED)
setBotanReqLib(PDJE_godot_wrapper)#botan req -- dept 3

setLibreMIDIReqLib(PDJE_godot_wrapper) #libremidi req -- dept 4
setOpenCLCppReqLib(PDJE_godot_wrapper)

if(LINUX)
  

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBEVDEV REQUIRED IMPORTED_TARGET libevdev)
pkg_check_modules(LIBWAYLAND_CLIENT REQUIRED IMPORTED_TARGET wayland-client)
pkg_check_modules(LIBNUMA REQUIRED IMPORTED_TARGET numa)
pkg_check_modules(LIBSYSTEMD REQUIRED IMPORTED_TARGET libsystemd)
endif(LINUX)
if(NOT APPLE)
PDJE_INPUT_LINK_LIB(PDJE_godot_wrapper) # input link req -- dept 5
endif()

find_package(spdlog CONFIG REQUIRED)
setSpdlogReqLib(PDJE_godot_wrapper) # spdlog req -- dept 6


if(PDJE_DEVELOP_INPUT)
target_link_libraries(PDJE_godot_wrapper PRIVATE PDJE_MODULE_INPUT PDJE_MODULE_JUDGE)

target_compile_definitions(PDJE_godot_wrapper PRIVATE PDJE_GODOT_ENABLE_INPUT_WRAPPER)
endif()

add_dependencies(PDJE_godot_wrapper godotcppEx)
# add_dependencies(PDJE_UTIL_BUNDLE godotcppEx)

if(WIN32)

target_compile_definitions(PDJE_godot_wrapper PRIVATE PDJE_WINDOWS_DLL)#check dll macro is valid.
endif()
target_include_directories( PDJE_godot_wrapper PRIVATE
    ${PDJE_WRAPPER_INCLUDE_DIRS}
)

target_compile_definitions(PDJE_godot_wrapper PRIVATE SPDLOG_FMT_EXTERNAL)


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


function(_pdje_get_target_prop out target prop)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR "Target not found: ${target}")
  endif()

  get_target_property(_tmp "${target}" "${prop}")
  if(NOT _tmp OR _tmp STREQUAL "_tmp-NOTFOUND")
    set(_tmp "")
  endif()

  set(${out} "${_tmp}" PARENT_SCOPE)
endfunction()

function(_pdje_normalize_list out)
  set(_items ${ARGN})

  set(_norm "")
  foreach(_x IN LISTS _items)
    if(NOT "${_x}" STREQUAL "")
      list(APPEND _norm "${_x}")
    endif()
  endforeach()

  list(REMOVE_DUPLICATES _norm)
  list(SORT _norm)
  set(${out} "${_norm}" PARENT_SCOPE)
endfunction()

function(_pdje_print_list_diff label a_name a_list b_name b_list)
  set(_only_a ${a_list})
  set(_only_b ${b_list})

  foreach(_x IN LISTS b_list)
    list(REMOVE_ITEM _only_a "${_x}")
  endforeach()

  foreach(_x IN LISTS a_list)
    list(REMOVE_ITEM _only_b "${_x}")
  endforeach()

  message(STATUS "========== ${label} ==========")

  if(_only_a)
    message(STATUS "[Only in ${a_name}]")
    foreach(_x IN LISTS _only_a)
      message(STATUS "  ${_x}")
    endforeach()
  else()
    message(STATUS "[Only in ${a_name}] <none>")
  endif()

  if(_only_b)
    message(STATUS "[Only in ${b_name}]")
    foreach(_x IN LISTS _only_b)
      message(STATUS "  ${_x}")
    endforeach()
  else()
    message(STATUS "[Only in ${b_name}] <none>")
  endif()
endfunction()

function(pdje_diff_targets_abi target_a target_b)
  if(NOT TARGET "${target_a}")
    message(FATAL_ERROR "Target not found: ${target_a}")
  endif()
  if(NOT TARGET "${target_b}")
    message(FATAL_ERROR "Target not found: ${target_b}")
  endif()

  set(_props
    COMPILE_OPTIONS
    COMPILE_DEFINITIONS
    INCLUDE_DIRECTORIES
    LINK_LIBRARIES
    LINK_OPTIONS
    INTERFACE_COMPILE_OPTIONS
    INTERFACE_COMPILE_DEFINITIONS
    INTERFACE_INCLUDE_DIRECTORIES
    INTERFACE_LINK_LIBRARIES
    INTERFACE_LINK_OPTIONS
  )

  message(STATUS "")
  message(STATUS "#############################################")
  message(STATUS "ABI/LINK DIFF: ${target_a} vs ${target_b}")
  message(STATUS "#############################################")

  foreach(_prop IN LISTS _props)
    _pdje_get_target_prop(_a_raw "${target_a}" "${_prop}")
    _pdje_get_target_prop(_b_raw "${target_b}" "${_prop}")

    _pdje_normalize_list(_a ${_a_raw})
    _pdje_normalize_list(_b ${_b_raw})

    _pdje_print_list_diff("${_prop}" "${target_a}" "${_a}" "${target_b}" "${_b}")
  endforeach()

  message(STATUS "######## Important ABI macro spot check ########")

  _pdje_get_target_prop(_a_defs_raw "${target_a}" "COMPILE_DEFINITIONS")
  _pdje_get_target_prop(_b_defs_raw "${target_b}" "COMPILE_DEFINITIONS")
  _pdje_get_target_prop(_a_idefs_raw "${target_a}" "INTERFACE_COMPILE_DEFINITIONS")
  _pdje_get_target_prop(_b_idefs_raw "${target_b}" "INTERFACE_COMPILE_DEFINITIONS")

  set(_a_defs_all ${_a_defs_raw} ${_a_idefs_raw})
  set(_b_defs_all ${_b_defs_raw} ${_b_idefs_raw})

  _pdje_normalize_list(_a_defs ${_a_defs_all})
  _pdje_normalize_list(_b_defs ${_b_defs_all})

  set(_abi_macros
    _GLIBCXX_USE_CXX11_ABI
    _GLIBCXX_DEBUG
    _GLIBCXX_ASSERTIONS
    _LIBCPP_ABI_VERSION
    _LIBCPP_DEBUG
    ROCKSDB_USE_RTTI
    ROCKSDB_NO_DYNAMIC_EXTENSION
    FMT_HEADER_ONLY
    SPDLOG_FMT_EXTERNAL
    SPDLOG_COMPILED_LIB
  )

  foreach(_m IN LISTS _abi_macros)
    set(_a_hit "")
    set(_b_hit "")

    foreach(_x IN LISTS _a_defs)
      if(_x MATCHES "^${_m}(=.*)?$")
        list(APPEND _a_hit "${_x}")
      endif()
    endforeach()

    foreach(_x IN LISTS _b_defs)
      if(_x MATCHES "^${_m}(=.*)?$")
        list(APPEND _b_hit "${_x}")
      endif()
    endforeach()

    if(_a_hit OR _b_hit)
      message(STATUS "Macro ${_m}:")
      message(STATUS "  ${target_a}: ${_a_hit}")
      message(STATUS "  ${target_b}: ${_b_hit}")
    endif()
  endforeach()

  message(STATUS "#############################################")
  message(STATUS "")
endfunction()

pdje_diff_targets_abi(PDJE PDJE_godot_wrapper)
