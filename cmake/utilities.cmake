include_guard(GLOBAL)

function(util_install_dependencies TARGET_FILE OUTPUT_DIR INCLUDE_SYSTEM_LIBRARIES)
	if(NOT BUILD_SHARED_LIBS)
		return()
	endif()
	
	## On windows, vcpkg automatically copies dependent shared library files
	## to the build and install directory, so nothing to do here.
	if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		return()
	
	## Unfortunately, this functionality is not available on linux. Instead, vcpkg
	## sets RPATH/RUNPATH, so that dependent .so files are found at runtime. But
	## the paths set by vcpkg are absolute, which means they are only correct for
	## the build directory. In order to have a self contained install directory as
	## well, we have to find and install all dependent .so files manually.
	elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
		## only CMAKE_INSTALL_PREFIX is available in the generated install script, so
		## we have to pass all other variables we want to use manually
		install(CODE "set(TARGET_FILE \"${TARGET_FILE}\")")
		install(CODE "set(OUTPUT_DIR \"${OUTPUT_DIR}\")")
		install(CODE "set(INCLUDE_SYSTEM_LIBRARIES \"${INCLUDE_SYSTEM_LIBRARIES}\")")
		install(CODE "set(CMAKE_BINARY_DIR \"${CMAKE_BINARY_DIR}\")")

		## this adds code to the install script, that recursively gets dependencies and
		## dependencies of dependencies and installs them to OUTPUT_DIR
		install(CODE [[
			cmake_policy(VERSION 3.22)

			function(install_dependencies LIBRARY)
				## skip already installed libraries
				get_property(INSTALLED_LIBS GLOBAL PROPERTY INSTALLED_LIBRARIES)
				if(LIBRARY IN_LIST INSTALLED_LIBS)
					return()
				endif()

				## skip system libraries if disabled
				string(FIND "${LIBRARY}" "${CMAKE_BINARY_DIR}" POSITION)
				if(NOT INCLUDE_SYSTEM_LIBRARIES AND POSITION EQUAL -1)
					return()
				endif()

				## install library
				file(INSTALL
					DESTINATION "${OUTPUT_DIR}"
					TYPE SHARED_LIBRARY
					FOLLOW_SYMLINK_CHAIN
					FILES "${LIBRARY}"
				)

				## mark library as installed
				set_property(GLOBAL APPEND PROPERTY INSTALLED_LIBRARIES "${LIBRARY}")

				## get dependencies of this library
				file(GET_RUNTIME_DEPENDENCIES
					LIBRARIES ${LIBRARY}
					RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
					UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
				)

				## recursively install dependencies
				foreach(FILE ${RESOLVED_DEPS})
					if(NOT IS_SYMLINK "${FILE}")
						install_dependencies("${FILE}")
					endif()
				endforeach()

				## report unresolved dependencies
				foreach(FILE ${UNRESOLVED_DEPS})
					message(STATUS "Unresolved from ${LIBRARY}: ${FILE}")
				endforeach()
			endfunction()

			## initialize a global list to store installed libraries
			set_property(GLOBAL PROPERTY INSTALLED_LIBRARIES "")

			## get runtime dependencies of the main executable
			file(GET_RUNTIME_DEPENDENCIES
				EXECUTABLES ${TARGET_FILE}
				RESOLVED_DEPENDENCIES_VAR RESOLVED_DEPS
				UNRESOLVED_DEPENDENCIES_VAR UNRESOLVED_DEPS
			)

			## install all resolved dependencies
			foreach(FILE ${RESOLVED_DEPS})
				install_dependencies(${FILE})
			endforeach()

			## report unresolved dependencies
			foreach(FILE ${UNRESOLVED_DEPS})
				message(STATUS "Unresolved: ${FILE}")
			endforeach()
		]])
	endif()
endfunction()

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
