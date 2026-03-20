
set(PDJE_INCLUDE_CORE
    ${PDJE_INSTALL_DIR}/include/core
  ${PDJE_INSTALL_DIR}/include/core/db
  ${PDJE_INSTALL_DIR}/include/core/audioRender
  ${PDJE_INSTALL_DIR}/include/core/audioRender/MixMachine
  ${PDJE_INSTALL_DIR}/include/core/audioRender/MixMachine/EFFECT
  ${PDJE_INSTALL_DIR}/include/core/audioRender/MixMachine/FAUST_VALS
  
  ${PDJE_INSTALL_DIR}/include/core/audioRender/MixMachine/MUSIC_CTR
  
  ${PDJE_INSTALL_DIR}/include/core/audioRender/MixMachine/MiniaudioObjects
  
  ${PDJE_INSTALL_DIR}/include/core/audioRender/MixMachine/INTERPOLATOR

  ${PDJE_INSTALL_DIR}/include/core/audioRender/ManualMix
  ${PDJE_INSTALL_DIR}/include/core/audioRender/ManualMix/ManualFausts
  ${PDJE_INSTALL_DIR}/include/core/db/Capnp/CapnpBinary
  ${PDJE_INSTALL_DIR}/include/core/db/Capnp/Translators
  ${PDJE_INSTALL_DIR}/include/core/db/Capnp/Translators/MixTranslator
  ${PDJE_INSTALL_DIR}/include/core/db/Capnp/Translators/MusicTranslator
  ${PDJE_INSTALL_DIR}/include/core/db/Capnp/Translators/NoteTranslator
    ${PDJE_BINARY_DIR}/_deps/miniaudio-src/extras/miniaudio_split
  ${PDJE_BINARY_DIR}/_deps/sql_amalgam-src
    
  ${highway_INCLUDE_DIR}

	${PDJE_INSTALL_DIR}/third_party/SoundTouch/soundtouch_include
	${PDJE_INSTALL_DIR}/third_party/Faust/compiled
  ${PDJE_INSTALL_DIR}/third_party/Faust/manual_compiled
  

  
  ${PDJE_INSTALL_DIR}/include/core/MainObjects
  ${PDJE_INSTALL_DIR}/include/core/MainObjects/audioPlayer
  ${PDJE_INSTALL_DIR}/include/core/MainObjects/editorObject
  ${PDJE_INSTALL_DIR}/include/core/MainObjects/tempDBObject
  

  ${PDJE_INSTALL_DIR}/include/core/interface



  ${PDJE_INSTALL_DIR}/include/core/editor
  ${PDJE_INSTALL_DIR}/include/core/editor/edit
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper/GitWrap
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper/GitWrap/Branch
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper/GitWrap/Commit
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper/GitWrap/Add
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper/GitWrap/Blame
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper/GitWrap/Diff
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper/GitWrap/Log
  ${PDJE_INSTALL_DIR}/include/core/editor/pdjeLinter
  
  ${PDJE_INSTALL_DIR}/include/core/editor/featureWrapper/JSONWrap
  ${PDJE_INSTALL_DIR}/include/core/editor/objects
  ${PDJE_INSTALL_DIR}/include/core/editor/renderer
  ${PDJE_BINARY_DIR}/capnpGen/third_party/Capnp
  
  ${PDJE_INSTALL_DIR}/include/input
    ${PDJE_BINARY_DIR}/_deps/nhjson-src/include
  ${PDJE_BINARY_DIR}/_deps/cppcodec-src
#   ${sql_amalgam_SOURCE_DIR}
#   ${cppcodec_SOURCE_DIR}
  )


set(PDJE_INCLUDE_JUDGE
${PDJE_INSTALL_DIR}/include/judge
${PDJE_INSTALL_DIR}/include/judge/NoteOBJ
${PDJE_INSTALL_DIR}/include/judge/Init
${PDJE_INSTALL_DIR}/include/judge/Loop
${PDJE_INSTALL_DIR}/include/global/Process
${CAPNPC_OUTPUT_DIR}/third_party/Capnp
)

set(PDJE_INCLUDE_INPUT
${PDJE_INSTALL_DIR}/include/input
${PDJE_INSTALL_DIR}/include/input/midi
${PDJE_INSTALL_DIR}/include/global/Process
${PDJE_INSTALL_DIR}/include/global/Process/utils

)

set(PDJE_INCLUDE_GLOBAL
  ${PDJE_INSTALL_DIR}/include/global
  ${PDJE_INSTALL_DIR}/include/global/DataLines
)
if(WIN32)
  list(APPEND PDJE_INCLUDE_GLOBAL
    ${PDJE_INSTALL_DIR}/include/global/Highres_Clock/Windows
  )
  list(APPEND PDJE_INCLUDE_INPUT
    ${PDJE_INSTALL_DIR}/include/input/windows
  )
elseif(APPLE)
  list(APPEND PDJE_INCLUDE_GLOBAL
    ${PDJE_INSTALL_DIR}/include/global/Highres_Clock/Mac
  )
  list(APPEND PDJE_INCLUDE_INPUT ${PDJE_INSTALL_DIR}/include/input/apple)
else()
  list(APPEND PDJE_INCLUDE_GLOBAL
    ${PDJE_INSTALL_DIR}/include/global/Highres_Clock/Linux
  )
  list(APPEND PDJE_INCLUDE_INPUT
    ${PDJE_INSTALL_DIR}/include/input/linux 
    ${PDJE_INSTALL_DIR}/include/input/linux/RT
    ${PDJE_INSTALL_DIR}/include/input/linux/common
    ${PDJE_INSTALL_DIR}/include/input/linux/socket
    
    )
endif()

list(APPEND PDJE_INCLUDE_JUDGE
  ${PDJE_INCLUDE_GLOBAL}
)
list(APPEND PDJE_INCLUDE_CORE
  ${PDJE_INCLUDE_GLOBAL}
)
list(APPEND PDJE_INCLUDE_INPUT
  ${PDJE_INCLUDE_GLOBAL}
)
