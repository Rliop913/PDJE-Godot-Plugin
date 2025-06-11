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

project(PDJE_godot_wrapper VERSION 0.1.0 LANGUAGES C CXX)


include(ExternalProject)
string(TOLOWER "${CMAKE_SYSTEM_NAME}" PLATFORM_ID_LOWER)
#-------------------

set(ARCH_LIST
    x86_32
    x86_64
    arm32
    arm64
    rv64
    ppc32
    ppc64
    wasm32
)


function(godot_arch_name OUTVAR)
    # Special case for macos universal builds that target both x86_64 and arm64
    if(DEFINED CMAKE_OSX_ARCHITECTURES)
        if("x86_64" IN_LIST CMAKE_OSX_ARCHITECTURES AND "arm64" IN_LIST CMAKE_OSX_ARCHITECTURES)
            set(${OUTVAR} "universal" PARENT_SCOPE)
            return()
        endif()
    endif()

    # Direct match early out.
    string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH)
    if(ARCH IN_LIST ARCH_LIST)
        set(${OUTVAR} "${ARCH}" PARENT_SCOPE)
        return()
    endif()

    # Known aliases
    set(x86_64 "w64;amd64;x86-64")
    set(arm32 "armv7;armv7-a")
    set(arm64 "armv8;arm64v8;aarch64;armv8-a")
    set(rv64 "rv;riscv;riscv64")
    set(ppc32 "ppcle;ppc")
    set(ppc64 "ppc64le")

    if(ARCH IN_LIST x86_64)
        set(${OUTVAR} "x86_64" PARENT_SCOPE)
    elseif(ARCH IN_LIST arm32)
        set(${OUTVAR} "arm32" PARENT_SCOPE)
    elseif(ARCH IN_LIST arm64)
        set(${OUTVAR} "arm64" PARENT_SCOPE)
    elseif(ARCH IN_LIST rv64)
        set(${OUTVAR} "rv64" PARENT_SCOPE)
    elseif(ARCH IN_LIST ppc32)
        set(${OUTVAR} "ppc32" PARENT_SCOPE)
    elseif(ARCH IN_LIST ppc64)
        set(${OUTVAR} "ppc64" PARENT_SCOPE)
    elseif(ARCH MATCHES "86")
        # Catches x86, i386, i486, i586, i686, etc.
        set(${OUTVAR} "x86_32" PARENT_SCOPE)
    else()
        # Default value is whatever the processor is.
        set(${OUTVAR} ${CMAKE_SYSTEM_PROCESSOR} PARENT_SCOPE)
    endif()
endfunction()



#-------------------
godot_arch_name(ARCH_LOWER)
# string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_LOWER)

set(GODOTCPP_PATH  "${PLATFORM_ID_LOWER}-${CMAKE_BUILD_TYPE}")
string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE_LOWER)

if(WIN32)

ExternalProject_Add(godot_cpp_external
  PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${GODOTCPP_PATH}
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/godot-cpp
  BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${GODOTCPP_PATH}
    -DTYPED_METHOD_BIND:BOOL=ON
    -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded$<$<CONFIG:Debug>:Debug>DLL

    BUILD_COMMAND ${CMAKE_COMMAND} --build . --config $<CONFIG>
    INSTALL_COMMAND ""
    # -DCMAKE_CXX_FLAGS=${MY_CUSTOM_FLAGS}
    # BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_$<CONFIG>.${ARCH_LOWER}${LIB_END}"
)
else()

ExternalProject_Add(godot_cpp_external
  PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${GODOTCPP_PATH}
  SOURCE_DIR      ${CMAKE_SOURCE_DIR}/godot-cpp
  BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${GODOTCPP_PATH}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

    INSTALL_COMMAND ""
    # -DCMAKE_CXX_FLAGS=${MY_CUSTOM_FLAGS}
    BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_${BUILD_TYPE_LOWER}.${ARCH_LOWER}.a"
)
endif()
add_library(godotcppEx STATIC IMPORTED GLOBAL)


if(WIN32)

set_target_properties(godotcppEx PROPERTIES
    # IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_$<IF:$<CONFIG:Debug,Release>:debug,release>.${ARCH_LOWER}.lib"
  IMPORTED_CONFIGURATIONS "Debug;Release"
  IMPORTED_LOCATION_DEBUG "${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_debug.${ARCH_LOWER}.lib"
  IMPORTED_LOCATION_RELEASE "${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_release.${ARCH_LOWER}.lib"
)

else()
set_target_properties(godotcppEx PROPERTIES
  IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_${BUILD_TYPE_LOWER}.${ARCH_LOWER}.a"
)
endif()
add_dependencies(godotcppEx godot_cpp_external)

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
target_link_libraries(PDJE_godot_wrapper PRIVATE PDJE godotcppEx
# ${CMAKE_BINARY_DIR}/bin/libgodot-cpp.linux.template_debug.x86_64.a
)

set_target_properties(PDJE_godot_wrapper PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/PDJE_Prebuilt"
)

message(${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/gen/include)
target_include_directories( PDJE_godot_wrapper PRIVATE
    ${CMAKE_SOURCE_DIR}/Wrapper_src
    ${CMAKE_SOURCE_DIR}/Wrapper_src/Player
    ${CMAKE_SOURCE_DIR}/Wrapper_src/Editor
    
    ${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/gen/include
    ${CMAKE_BINARY_DIR}/_build/godotcpp/${GODOTCPP_PATH}/gen/include/godot_cpp
    ${CMAKE_SOURCE_DIR}/godot-cpp/include
    ${CMAKE_SOURCE_DIR}/godot-cpp/include/godot_cpp
    ${CMAKE_SOURCE_DIR}/godot-cpp/gdextension
    
    
    
  
  
  
)
