foreach(i ${CMAKE_SYSTEM_PREFIX_PATH})
	message(CHECK_START "Finding file2c: ${i}/file2c/bin")

	find_program(FILE2C_EXECUTABLE file2c NAMES file2c HINTS ${i}/file2c/bin)

	if (FILE2C_EXECUTABLE)
		message(CHECK_PASS "Found file2c: ${RE2C_EXECUTABLE}")
		break()
	endif()
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    file2c
    REQUIRED_VARS FILE2C_EXECUTABLE)

if(FILE2C_EXECUTABLE)
    macro(add_file2c_target)
		set(_options "")
		set(_oneValueArgs TARGET_NAME INPUT_FILE OUTPUT_FILE VAR_NAME PREFIXES)
		set(_multiValueArgs INCLUDE_FILES)
		cmake_parse_arguments(_file2c
		"${_options}"
		"${_oneValueArgs}"
		"${_multiValueArgs}"
		${ARGN})

		if(NOT _file2c_TARGET_NAME)
			message(FATAL_ERROR "The caller must provide the target name for file2c targets")
		endif()
		
		if(NOT _file2c_INPUT_FILE)
			message(FATAL_ERROR "Missing input file name")
		endif()
		
		if(NOT _file2c_OUTPUT_FILE)
			message(FATAL_ERROR "Missing output file name")
		endif()
		
		if(NOT _file2c_VAR_NAME)
			message(FATAL_ERROR "Missing variable name")
		endif()

		set(_file2c_full_command ${FILE2C_EXECUTABLE}
			${_file2c_INPUT_FILE}
			-o ${_file2c_OUTPUT_FILE}
			-n ${_file2c_VAR_NAME})

		if(_file2c_PREFIXES)
			set(_file2c_full_command ${_file2c_full_command} -p ${_file2c_PREFIXES})
		endif()

		foreach(i ${_file2c_INCLUDE_FILES})
			set(_file2c_full_command ${_file2c_full_command} -I ${i})
		endforeach()

        add_custom_target(
			${_file2c_TARGET_NAME}
            COMMAND ${_file2c_full_command}
            BYPRODUCTS ${_file2c_OUTPUT_FILE}
            SOURCES ${_file2c_INPUT_FILE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			VERBATIM
        )
    endmacro()
endif()
