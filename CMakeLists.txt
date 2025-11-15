cmake_minimum_required(VERSION 3.28)
project(BlastEngine VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

if(WIN32)
add_subdirectory("C:/glfw-3.4/" glfw3)
elseif(UNIX)
find_package(glfw3 3.4 REQUIRED)
endif()

find_package(Vulkan REQUIRED)

add_executable(BlastEngine)
add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(ext/vk-bootstrap)

target_include_directories(BlastEngine PRIVATE include)
target_link_libraries(BlastEngine PRIVATE 
	glfw
	Vulkan::Vulkan
	vk-bootstrap::vk-bootstrap
)

### SHADER PART ###
#based on this CMAKE video https://www.youtube.com/watch?v=ZuHK_5cJ6B8&t=217s

find_program(SLANG_COMPILATOR slangc)
find_program(GLSL_COMPILATOR glslc)

file(GLOB_RECURSE SLANG_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.slang"
)
 
foreach(SLANG ${SLANG_SOURCE_FILES})
	get_filename_component(FILE_NAME ${SLANG} NAME_WLE)
	set(SHADER_PATH "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}")
	if(${FILE_NAME} MATCHES Module$)
		set(BINARY_FILE "${SHADER_PATH}.slang-module")
		add_custom_command(
			OUTPUT ${BINARY_FILE}
			COMMAND ${SLANG_COMPILATOR} ${SLANG} -o ${BINARY_FILE}
			DEPENDS ${SLANG})
			list(APPEND MODULE_FILES ${BINARY_FILE})
	else()
		set(ENTRY_POINT -entry fragmentMain -entry vertexMain)
		set(BINARY_FILE "${SHADER_PATH}.spv")
		add_custom_command(
  		OUTPUT ${BINARY_FILE}  
			COMMAND ${SLANG_COMPILATOR} ${SLANG} -target spirv -profile spirv_1_4 ${ENTRY_POINT} -o ${BINARY_FILE}
			DEPENDS ${SLANG}
			VERBATIM)
			list(APPEND SHADER_FILES ${BINARY_FILE})
	endif()
endforeach(SLANG)

add_custom_target(
	Shaders
	DEPENDS ${MODULE_FILES} 
	DEPENDS ${SHADER_FILES}
)

add_dependencies(BlastEngine 
	Shaders
)