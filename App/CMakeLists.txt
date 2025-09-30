cmake_minimum_required(VERSION 3.22)

project("App Project")

## name of target binary, component and runtime dependency set
set(TARGET_NAME "app")
set(COMPONENT_NAME "${TARGET_NAME}")
set(RUNTIME_DEP_SET_NAME "${TARGET_NAME}_runtime_deps")

## header files
file(GLOB_RECURSE INC_FILES "include/*.h")

## source files
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

## add source and header files to this project's target binary
add_executable("${TARGET_NAME}" ${SRC_FILES} ${INC_FILES})

## on windows, don't open the console window for release builds
if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND CMAKE_BUILD_TYPE STREQUAL "Release")
	## when using WIN32_EXECUTABLE ON, the linker expects a WinMain entry point, so
	## we have to set /ENTRY manually to use the default main entry point instead
	set_target_properties("${TARGET_NAME}" PROPERTIES WIN32_EXECUTABLE ON)
	target_link_options("${TARGET_NAME}" PRIVATE "/ENTRY:mainCRTStartup")
endif()

## add include directories
target_include_directories("${TARGET_NAME}" PRIVATE "include")

## add libraries
# via vcpkg
#find_package(<lib> CONFIG REQUIRED)
#target_link_libraries("${TARGET_NAME}" PRIVATE <lib>)
#
# manually
#add_library(<lib> SHARED IMPORTED)
#set_target_properties(<lib> PROPERTIES
#	IMPORTED_IMPLIB <path to .lib/.a>
#	IMPORTED_LOCATION <path to .dll/.so>
#	INTERFACE_INCLUDE_DIRECTORIES <path to include>
#)
#target_link_libraries("${TARGET_FILE}" PRIVATE <lib>)
#add_custom_command(TARGET "${TARGET_FILE}" POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_if_different
#	"$<TARGET_FILE:<lib>>" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
#	VERBATIM
#)
#install(IMPORTED_RUNTIME_ARTIFACTS <lib>
#	RUNTIME_DEPENDENCY_SET "${RUNTIME_DEP_SET_NAME}"
#	RUNTIME DESTINATION "${RUNTIME_INSTALL_DIRECTORY}"
#	LIBRARY DESTINATION "${LIBRARY_INSTALL_DIRECTORY}"
#	COMPONENT "${COMPONENT_NAME}"
#)

# glew
find_package(GLEW CONFIG REQUIRED)
target_link_libraries("${TARGET_NAME}" PRIVATE GLEW::GLEW)

# glfw3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries("${TARGET_NAME}" PRIVATE glfw)

# glm
find_package(glm CONFIG REQUIRED)
target_link_libraries("${TARGET_NAME}" PRIVATE glm::glm)

# freetype
find_package(freetype CONFIG REQUIRED)
target_link_libraries("${TARGET_NAME}" PRIVATE freetype)

# assimp
find_package(assimp CONFIG REQUIRED)
target_link_libraries("${TARGET_NAME}" PRIVATE assimp::assimp)

# devil
find_package(DevIL REQUIRED)
target_link_libraries("${TARGET_NAME}" PRIVATE DevIL::IL)
target_link_libraries("${TARGET_NAME}" PRIVATE DevIL::ILU)
target_link_libraries("${TARGET_NAME}" PRIVATE DevIL::ILUT)

# for some reason, these devil dependencies are not linked automatically
# when building as static library
if(NOT BUILD_SHARED_LIBS)
	add_compile_definitions(IL_STATIC_LIB)

	find_package(Jasper REQUIRED)
	target_link_libraries("${TARGET_NAME}" PRIVATE Jasper::Jasper)

	find_package(libjpeg-turbo CONFIG REQUIRED)
	target_link_libraries("${TARGET_NAME}" PRIVATE libjpeg-turbo::turbojpeg-static)

	find_package(TIFF REQUIRED)
	target_link_libraries("${TARGET_NAME}" PRIVATE TIFF::TIFF)
endif()

## make assets available in the build directory via symlink
util_add_post_build_create_symlink("${TARGET_NAME}"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/shader"
	"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/src/shader"
)
util_add_post_build_create_symlink("${TARGET_NAME}"
	"${CMAKE_CURRENT_SOURCE_DIR}/resources"
	"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources"
)

## install executable, shared and static libraries
install(TARGETS "${TARGET_NAME}"
	RUNTIME_DEPENDENCY_SET "${RUNTIME_DEP_SET_NAME}"
	RUNTIME DESTINATION "${RUNTIME_INSTALL_DIRECTORY}"
	LIBRARY DESTINATION "${LIBRARY_INSTALL_DIRECTORY}"
	ARCHIVE DESTINATION "${ARCHIVE_INSTALL_DIRECTORY}"
	COMPONENT "${COMPONENT_NAME}"
)
install(RUNTIME_DEPENDENCY_SET "${RUNTIME_DEP_SET_NAME}"
	DESTINATION "${LIBRARY_INSTALL_DIRECTORY}"
	COMPONENT "${COMPONENT_NAME}"
	PRE_EXCLUDE_REGEXES ${GLOBAL_DEP_PRE_EXCLUDE_REGEXES}
	POST_EXCLUDE_REGEXES ${GLOBAL_DEP_POST_EXCLUDE_REGEXES}
)

## install assets
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/shader" DESTINATION "${RUNTIME_INSTALL_DIRECTORY}/src")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources" DESTINATION "${RUNTIME_INSTALL_DIRECTORY}")
