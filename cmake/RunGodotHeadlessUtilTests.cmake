if(NOT DEFINED PDJE_GODOT_PROJECT_DIR OR
   "${PDJE_GODOT_PROJECT_DIR}" STREQUAL "")
  message(FATAL_ERROR "PDJE_GODOT_PROJECT_DIR is required")
endif()

if("${PDJE_GODOT_EXECUTABLE}" STREQUAL "" AND
   DEFINED ENV{PDJE_GODOT_EXECUTABLE})
  set(PDJE_GODOT_EXECUTABLE "$ENV{PDJE_GODOT_EXECUTABLE}")
endif()

if("${PDJE_GODOT_EXECUTABLE}" STREQUAL "")
  find_program(
    PDJE_GODOT_EXECUTABLE
    NAMES godot godot4 godot.exe godot4.exe
  )
endif()

if(NOT PDJE_GODOT_EXECUTABLE OR
   NOT EXISTS "${PDJE_GODOT_EXECUTABLE}")
  message(FATAL_ERROR
    "Godot executable not found. Configure with "
    "-DPDJE_GODOT_EXECUTABLE=<path> or set the "
    "PDJE_GODOT_EXECUTABLE environment variable.")
endif()

execute_process(
  COMMAND "${PDJE_GODOT_EXECUTABLE}"
          --headless
          --path "${PDJE_GODOT_PROJECT_DIR}"
          --script "res://tests/headless/util_api_test.gd"
  WORKING_DIRECTORY "${PDJE_GODOT_PROJECT_DIR}"
  RESULT_VARIABLE _pdje_godot_result
  COMMAND_ECHO STDOUT
)

if(NOT _pdje_godot_result EQUAL 0)
  message(FATAL_ERROR
    "Godot headless Util API test failed with exit code "
    "${_pdje_godot_result}")
endif()
