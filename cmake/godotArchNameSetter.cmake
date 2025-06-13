

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