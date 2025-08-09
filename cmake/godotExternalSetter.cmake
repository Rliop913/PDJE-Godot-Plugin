

include(cmake/godotArchNameSetter.cmake)

if(WIN32)

    set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-debug")
    ExternalProject_Add(godot_cpp_external_debug
    PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${PLATFORM_BUILD_PATH}
    GIT_REPOSITORY  https://github.com/godotengine/godot-cpp.git
    GIT_TAG         godot-4.4.1-stable
    BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${PLATFORM_BUILD_PATH}
        -DTYPED_METHOD_BIND:BOOL=ON
        -DGODOTCPP_DEBUG_CRT:BOOL=ON
        -DGODOTCPP_USE_STATIC_CPP:BOOL=OFF
        -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebugDLL

        BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Debug
        INSTALL_COMMAND ""
    )

    set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-release")
    ExternalProject_Add(godot_cpp_external_release
    PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${PLATFORM_BUILD_PATH}
    GIT_REPOSITORY  https://github.com/godotengine/godot-cpp.git
    GIT_TAG         godot-4.4.1-stable
    BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${PLATFORM_BUILD_PATH}
        -DTYPED_METHOD_BIND:BOOL=ON
        -DGODOTCPP_DEBUG_CRT=OFF
        -DGODOTCPP_USE_STATIC_CPP=OFF
        -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL
        -DGODOTCPP_TARGET=template_release

        BUILD_COMMAND ${CMAKE_COMMAND} --build . --config Release
        INSTALL_COMMAND ""
    )
    
    ExternalProject_Get_Property(godot_cpp_external_release source_dir binary_dir install_dir)
elseif(APPLE)

    set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-${CMAKE_BUILD_TYPE}")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(GODOTCPP_USE_HOT_RELOAD OFF CACHE BOOL "Disable hot reload" FORCE)
        ExternalProject_Add(godot_cpp_external
        PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${PLATFORM_BUILD_PATH}
        GIT_REPOSITORY  https://github.com/godotengine/godot-cpp.git
        GIT_TAG         godot-4.4.1-stable
        BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${PLATFORM_BUILD_PATH}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DGODOTCPP_TARGET=template_${BUILD_TYPE_LOWER}
            -DGODOTCPP_USE_HOT_RELOAD=OFF

            INSTALL_COMMAND ""
            BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/bin/libgodot-cpp.macos.template_${BUILD_TYPE_LOWER}.${ARCH_LOWER}.a"
        )
    else()
        ExternalProject_Add(godot_cpp_external
        PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${PLATFORM_BUILD_PATH}
        GIT_REPOSITORY  https://github.com/godotengine/godot-cpp.git
        GIT_TAG         godot-4.4.1-stable
        BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${PLATFORM_BUILD_PATH}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DGODOTCPP_TARGET=template_${BUILD_TYPE_LOWER}

            INSTALL_COMMAND ""
            BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/bin/libgodot-cpp.macos.template_${BUILD_TYPE_LOWER}.${ARCH_LOWER}.a"
        )
    endif()
    ExternalProject_Get_Property(godot_cpp_external source_dir binary_dir install_dir)
else()
    set(PLATFORM_BUILD_PATH  "${PLATFORM_ID_LOWER}-${CMAKE_BUILD_TYPE}")
    ExternalProject_Add(godot_cpp_external
    PREFIX          ${CMAKE_BINARY_DIR}/_deps/godotcpp/${PLATFORM_BUILD_PATH}
    GIT_REPOSITORY  https://github.com/godotengine/godot-cpp.git
    GIT_TAG         godot-4.4.1-stable
    BINARY_DIR      ${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/_install/godotcpp/${PLATFORM_BUILD_PATH}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGODOTCPP_TARGET=template_${BUILD_TYPE_LOWER}
        
        INSTALL_COMMAND ""
        BUILD_BYPRODUCTS "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_${BUILD_TYPE_LOWER}.${ARCH_LOWER}.a"
    )
    ExternalProject_Get_Property(godot_cpp_external source_dir binary_dir install_dir)
endif()
add_library(godotcppEx STATIC IMPORTED GLOBAL)
if(WIN32)

    set_target_properties(godotcppEx PROPERTIES
    IMPORTED_CONFIGURATIONS "Debug;Release"
    IMPORTED_LOCATION_DEBUG "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_ID_LOWER}-debug/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_debug.${ARCH_LOWER}.lib"
    IMPORTED_LOCATION_RELEASE "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_ID_LOWER}-release/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_release.${ARCH_LOWER}.lib"
        MSVC_RUNTIME_LIBRARY "$<$<CONFIG:Debug>:MultiThreadedDebugDLL>$<$<CONFIG:Release>:MultiThreadedDLL>"
    )
    add_dependencies(godotcppEx godot_cpp_external_debug godot_cpp_external_release)
elseif(APPLE)
set_target_properties(godotcppEx PROPERTIES
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/bin/libgodot-cpp.macos.template_${BUILD_TYPE_LOWER}.${ARCH_LOWER}.a"
    )
else()
    set_target_properties(godotcppEx PROPERTIES
    IMPORTED_LOCATION "${CMAKE_BINARY_DIR}/_build/godotcpp/${PLATFORM_BUILD_PATH}/bin/libgodot-cpp.${PLATFORM_ID_LOWER}.template_${BUILD_TYPE_LOWER}.${ARCH_LOWER}.a"
    )
    add_dependencies(godotcppEx godot_cpp_external)
endif()