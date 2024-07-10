cmake_minimum_required(VERSION 3.8)

## enable hot reload for MSVC compilers if supported.
if(POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
		"$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>"
	)
endif()

project("OpenGL Template Project")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## target binary file
set(TARGET_FILE
	"Cpp-OpenGL-Application"
)

## header files
set(INC_FILES
	"include/camera.h"
	"include/image.h"
	"include/mesh.h"
	"include/model.h"
	"include/shader.h"
	"include/textRenderer.h"
	"include/texture.h"
	"include/window.h"
)

## source files
set(SRC_FILES
	"src/camera.cpp"
	"src/image.cpp"
	"src/main.cpp"
	"src/mesh.cpp"
	"src/model.cpp"
	"src/shader.cpp"
	"src/textRenderer.cpp"
	"src/texture.cpp"
	"src/window.cpp"
)

## shader files
set(SHADER_FILES
	"src/shader/lamp.frag"
	"src/shader/lamp.vert"
	"src/shader/main.frag"
	"src/shader/main.vert"
	"src/shader/text.frag"
	"src/shader/text.vert"
)

## resource files
set(RESOURCE_FILES
	"resources/font/consola.ttf"
	"resources/texture/piston.png"
	"resources/objects/backpack/ao.jpg"
	"resources/objects/backpack/backpack.mtl"
	"resources/objects/backpack/backpack.obj"
	"resources/objects/backpack/diffuse.jpg"
	"resources/objects/backpack/normal.png"
	"resources/objects/backpack/roughness.jpg"
	"resources/objects/backpack/specular.jpg"
	"resources/objects/container/container.mtl"
	"resources/objects/container/container.obj"
	"resources/objects/container/container.png"
	"resources/objects/lamp/lamp.mtl"
	"resources/objects/lamp/lamp.obj"
)

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

# devil
if(WIN32)
	find_package(DevIL REQUIRED)
	target_link_libraries(${TARGET_FILE} PRIVATE DevIL::IL)
	target_link_libraries(${TARGET_FILE} PRIVATE DevIL::ILU)
	target_link_libraries(${TARGET_FILE} PRIVATE DevIL::ILUT)
else()
	find_package(PkgConfig REQUIRED)
	
	pkg_check_modules(IL REQUIRED IMPORTED_TARGET IL)
	target_link_libraries(${TARGET_FILE} PRIVATE PkgConfig::IL)
	
	pkg_check_modules(ILU REQUIRED IMPORTED_TARGET ILU)
	target_link_libraries(${TARGET_FILE} PRIVATE PkgConfig::ILU)
	
	pkg_check_modules(ILUT REQUIRED IMPORTED_TARGET ILUT)
	target_link_libraries(${TARGET_FILE} PRIVATE PkgConfig::ILUT)
	
	pkg_check_modules(jasper REQUIRED IMPORTED_TARGET jasper)
	target_link_libraries(${TARGET_FILE} PRIVATE PkgConfig::jasper)
	
	#find_package(libjpeg-turbo CONFIG REQUIRED)
	#target_link_libraries(${TARGET_FILE} PRIVATE libjpeg-turbo::turbojpeg-static)

	pkg_check_modules(libtiff-4 REQUIRED IMPORTED_TARGET libtiff-4)
	target_link_libraries(${TARGET_FILE} PRIVATE PkgConfig::libtiff-4)
endif()

# freetype
find_package(freetype CONFIG REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE freetype)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(${TARGET_FILE} PRIVATE assimp::assimp)

## do not open console window
#set_target_properties(${TARGET_FILE} PROPERTIES WIN32_EXECUTABLE ON)

## copy shader files to build directory
add_custom_target(copy_shaders ALL DEPENDS ${SHADER_FILES})
add_dependencies(${TARGET_FILE} copy_shaders)
foreach(SHADER_FILE ${SHADER_FILES})
	add_custom_command(
		TARGET copy_shaders
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_SOURCE_DIR}/${SHADER_FILE}
		${CMAKE_CURRENT_BINARY_DIR}/${SHADER_FILE}
	)
endforeach()

## copy resource files to build directory
#add_custom_target(copy_resources ALL DEPENDS ${RESOURCE_FILES})
#add_dependencies(${TARGET_FILE} copy_resources)
#foreach(RESOURCE_FILE ${RESOURCE_FILES})
#	add_custom_command(
#		TARGET copy_resources
#		COMMAND ${CMAKE_COMMAND} -E copy_if_different
#		${CMAKE_CURRENT_SOURCE_DIR}/${RESOURCE_FILE}
#		${CMAKE_CURRENT_BINARY_DIR}/${RESOURCE_FILE}
#	)
#endforeach()
add_custom_target(copy_resources ALL
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_SOURCE_DIR}/resources
	${CMAKE_CURRENT_BINARY_DIR}/resources
)
add_dependencies(${TARGET_FILE} copy_resources)

## copy dynamic library, shader and resource files from build to install directory
install(TARGETS ${TARGET_FILE} DESTINATION ${CMAKE_INSTALL_PREFIX})
if(NOT "${SHADER_FILES}" STREQUAL "")
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
if(NOT "${RESOURCE_FILES}" STREQUAL "")
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/resources DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
