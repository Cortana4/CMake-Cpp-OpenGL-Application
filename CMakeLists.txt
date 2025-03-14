cmake_minimum_required(VERSION 3.22)

project("OpenGL Template Project")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## target binary file
set(TARGET_FILE "Cpp-OpenGL-Application")

## header files
file(GLOB_RECURSE INC_FILES "include/*.h")

## source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

## set build directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${RUNTIME_SUBDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${LIBRARY_SUBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ARCHIVE_SUBDIR}")

## set install directories
set(RUNTIME_INSTALL_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${RUNTIME_SUBDIR}")
set(LIBRARY_INSTALL_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${LIBRARY_SUBDIR}")
set(ARCHIVE_INSTALL_DIRECTORY "${CMAKE_INSTALL_PREFIX}/${ARCHIVE_SUBDIR}")

## add source and header files to this project's executable
add_executable(${TARGET_FILE} ${SRC_FILES} ${INC_FILES})

## add include directories
target_include_directories(${TARGET_FILE} PRIVATE "include")

## add libraries
# glew
find_package(GLEW CONFIG REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE GLEW::GLEW)

# glfw3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE glfw)

# glm
find_package(glm CONFIG REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE glm::glm)

# freetype
find_package(freetype CONFIG REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE freetype)

# assimp
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE assimp::assimp)

# devil
find_package(DevIL REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE DevIL::IL)
target_link_libraries(${TARGET_FILE} PRIVATE DevIL::ILU)
target_link_libraries(${TARGET_FILE} PRIVATE DevIL::ILUT)

# for some reason, these devil dependencies are not linked automatically
# when building as static library
if(NOT BUILD_SHARED_LIBS)
	add_compile_definitions(IL_STATIC_LIB)

	find_package(Jasper REQUIRED)
	target_link_libraries(${TARGET_FILE} PRIVATE Jasper::Jasper)

	find_package(libjpeg-turbo CONFIG REQUIRED)
	target_link_libraries(${TARGET_FILE} PRIVATE libjpeg-turbo::turbojpeg-static)

	find_package(TIFF REQUIRED)
	target_link_libraries(${TARGET_FILE} PRIVATE TIFF::TIFF)
endif()

# freetype
find_package(freetype CONFIG REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE freetype)


if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	## on windows, open the console window only for debug builds
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set_target_properties(${TARGET_FILE} PROPERTIES WIN32_EXECUTABLE OFF)
	else()
		set_target_properties(${TARGET_FILE} PROPERTIES WIN32_EXECUTABLE ON)
	endif()

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	if(BUILD_SHARED_LIBS)
		## On Windows, vcpkg automatically copies dependent shared library files
		## to the build and install directory. Unfortunately, this functionality
		## is not available on linux. Instead, vcpkg sets RPATH/RUNPATH, so that
		## dependent .so files are found at runtime. But the paths set by vcpkg
		## are absolute, which means they are only correct for the build directory.
		## In order to have a self contained install directory, we have to find
		## and install all dependent .so files manually.

		## only CMAKE_INSTALL_PREFIX is available in the generated install script, so
		## we have to pass all other variables we want to use manually
		install(CODE "set(TARGET_FILE \"${RUNTIME_SUBDIR}/${TARGET_FILE}\")")
		install(CODE "set(LIBRARY_INSTALL_DIRECTORY \"${LIBRARY_INSTALL_DIRECTORY}\")")
		install(CODE "set(INSTALL_SYSTEM_LIBRARIES \"${INSTALL_SYSTEM_LIBRARIES}\")")
		install(CODE "set(CMAKE_BINARY_DIR \"${CMAKE_BINARY_DIR}\")")

		## this adds code to the install script, that recursively gets dependencies and
		## dependencies of dependencies and installs them to LIBRARY_INSTALL_DIRECTORY
		install(CODE [[
			cmake_policy(VERSION 3.22)

			function(install_dependencies LIBRARY)
				## skip already installed libraries
				get_property(INSTALLED_LIBS GLOBAL PROPERTY INSTALLED_LIBRARIES)
				if(LIBRARY IN_LIST INSTALLED_LIBS)
					return()
				endif()

				## skip system libraries if enabled
				string(FIND "${LIBRARY}" "${CMAKE_BINARY_DIR}" POSITION)
				if(NOT INSTALL_SYSTEM_LIBRARIES AND POSITION EQUAL -1)
					return()
				endif()

				## install library
				file(INSTALL
					DESTINATION "${LIBRARY_INSTALL_DIRECTORY}"
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
					if(NOT IS_SYMLINK ${FILE})
						install_dependencies(${FILE})
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
endif()

## install exetutable, shared and static libraries
install(TARGETS ${TARGET_FILE} RUNTIME DESTINATION ${RUNTIME_INSTALL_DIRECTORY})
install(TARGETS ${TARGET_FILE} LIBRARY DESTINATION ${LIBRARY_INSTALL_DIRECTORY})
install(TARGETS ${TARGET_FILE} ARCHIVE DESTINATION ${ARCHIVE_INSTALL_DIRECTORY})

## install shader folder
install(DIRECTORY src/shader DESTINATION ${RUNTIME_INSTALL_DIRECTORY}/src)

## install resources folder
install(DIRECTORY resources DESTINATION ${RUNTIME_INSTALL_DIRECTORY})
