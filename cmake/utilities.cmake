include_guard(GLOBAL)

function(util_add_post_build_create_symlink TARGET_NAME SRC_DIR DST_DIR)
	cmake_path(GET DST_DIR PARENT_PATH DST_PARENT_DIR)

	## the built-in cmake command "create_symlink" needs privileges on windows,
	## so we create a junction using native tools instead
	if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		cmake_path(NATIVE_PATH SRC_DIR SRC_NATIVE_DIR)
		cmake_path(NATIVE_PATH DST_DIR DST_NATIVE_DIR)

		add_custom_command(TARGET "${TARGET_NAME}" POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${DST_PARENT_DIR}"
			COMMAND ${CMAKE_COMMAND} -E rm -rf "${DST_DIR}"
			COMMAND cmd /c mklink /j "${DST_NATIVE_DIR}" "${SRC_NATIVE_DIR}"
			VERBATIM
		)
	## on linux we use the built-in create_symlink command
	else()
		add_custom_command(TARGET "${TARGET_NAME}" POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${DST_PARENT_DIR}"
			COMMAND ${CMAKE_COMMAND} -E rm -rf "${DST_DIR}"
			COMMAND ${CMAKE_COMMAND} -E create_symlink "${SRC_DIR}" "${DST_DIR}"
			VERBATIM
		)
	endif()
endfunction()
