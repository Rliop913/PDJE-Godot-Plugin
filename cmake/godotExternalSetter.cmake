

include(cmake/godotArchNameSetter.cmake)

if(BUILD_TYPE_LOWER STREQUAL "debug")
    set(GODOTCPP_TARGET "template_debug")
else()
    set(GODOTCPP_TARGET "template_release")
endif()

if(WIN32)

    set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-${BUILD_TYPE_LOWER}")
    if(GODOTCPP_TARGET STREQUAL "template_debug")
        set(GODOTCPP_DEBUG_CRT ON)
        set(GODOTCPP_MSVC_RUNTIME_LIBRARY "MultiThreadedDebugDLL")
    else()
        set(GODOTCPP_DEBUG_CRT OFF)
        set(GODOTCPP_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
    endif()
    set(GODOTCPP_LIBRARY "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.${GODOTCPP_TARGET}.${ARCH_LOWER}.lib")

    ExternalProject_Add(godot_cpp_external
    PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${PLATFORM_BUILD_PATH}
    GIT_REPOSITORY  https://github.com/godotengine/godot-cpp.git
    GIT_TAG         godot-4.5-stable
    BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${PLATFORM_BUILD_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DTYPED_METHOD_BIND:BOOL=ON
        -DGODOTCPP_DEBUG_CRT:BOOL=${GODOTCPP_DEBUG_CRT}
        -DGODOTCPP_USE_STATIC_CPP:BOOL=OFF
        -DCMAKE_MSVC_RUNTIME_LIBRARY=${GODOTCPP_MSVC_RUNTIME_LIBRARY}
        -DGODOTCPP_TARGET=${GODOTCPP_TARGET}

    BUILD_COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${GODOTCPP_LIBRARY}"
    )

    ExternalProject_Get_Property(godot_cpp_external source_dir binary_dir install_dir)
elseif(APPLE)

    set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-${BUILD_TYPE_LOWER}")
    set(GODOTCPP_LIBRARY "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/bin/libgodot-cpp.macos.${GODOTCPP_TARGET}.${ARCH_LOWER}.a")
    ExternalProject_Add(godot_cpp_external
    PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${PLATFORM_BUILD_PATH}
    GIT_REPOSITORY  https://github.com/godotengine/godot-cpp.git
    GIT_TAG         godot-4.5-stable
    BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${PLATFORM_BUILD_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGODOTCPP_TARGET=${GODOTCPP_TARGET}
        -DGODOTCPP_USE_HOT_RELOAD:BOOL=OFF

    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${GODOTCPP_LIBRARY}"
    )
    ExternalProject_Get_Property(godot_cpp_external source_dir binary_dir install_dir)
else()
    set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-${BUILD_TYPE_LOWER}")
    set(GODOTCPP_LIBRARY "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.${GODOTCPP_TARGET}.${ARCH_LOWER}.a")
    ExternalProject_Add(godot_cpp_external
    PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${PLATFORM_BUILD_PATH}
    GIT_REPOSITORY  https://github.com/godotengine/godot-cpp.git
    GIT_TAG         godot-4.5-stable
    BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${PLATFORM_BUILD_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGODOTCPP_TARGET=${GODOTCPP_TARGET}
        -DCMAKE_C_COMPILER=clang
        -DCMAKE_CXX_COMPILER=clang++
        
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS "${GODOTCPP_LIBRARY}"
    )
    ExternalProject_Get_Property(godot_cpp_external source_dir binary_dir install_dir)
endif()
add_library(godotcppEx STATIC IMPORTED GLOBAL)
if(WIN32)

    set_target_properties(godotcppEx PROPERTIES
    IMPORTED_LOCATION "${GODOTCPP_LIBRARY}"
        MSVC_RUNTIME_LIBRARY "${GODOTCPP_MSVC_RUNTIME_LIBRARY}"
    )
    add_dependencies(godotcppEx godot_cpp_external)
elseif(APPLE)
set_target_properties(godotcppEx PROPERTIES
    IMPORTED_LOCATION "${GODOTCPP_LIBRARY}"
    )
    add_dependencies(godotcppEx godot_cpp_external)
else()
    set_target_properties(godotcppEx PROPERTIES
    IMPORTED_LOCATION "${GODOTCPP_LIBRARY}"
    )
    add_dependencies(godotcppEx godot_cpp_external)
endif()
