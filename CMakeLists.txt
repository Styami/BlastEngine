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

target_include_directories(BlastEngine PRIVATE include)
target_link_libraries(BlastEngine PRIVATE 
	glfw
	Vulkan::Vulkan
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
	if(${FILE_NAME} MATCHES Vert$)
	  	set(GLSL_FILE "${SHADER_PATH}.vert")
	  	set(BINARY_FILE "${SHADER_PATH}.spv")
		add_custom_command(
  		OUTPUT ${GLSL_FILE} ${BINARY_FILE}  
			COMMAND ${SLANG_COMPILATOR} -target glsl -profile glsl_450 -entry vertexMain ${SLANG} -o ${GLSL_FILE}
			COMMAND ${GLSL_COMPILATOR} ${GLSL_FILE} -o ${BINARY_FILE}
			DEPENDS ${SLANG})
  		list(APPEND BINARY_FILES ${GLSL_FILE})
	elseif(${FILE_NAME} MATCHES Frag$)
	  	set(GLSL_FILE "${SHADER_PATH}.frag")
	  	set(BINARY_FILE "${SHADER_PATH}.spv")
		add_custom_command(
			OUTPUT ${GLSL_FILE} ${BINARY_FILE}
			COMMAND ${SLANG_COMPILATOR} -target glsl -profile glsl_450 -entry fragmentMain ${SLANG} -o "${GLSL_FILE}"
			COMMAND ${GLSL_COMPILATOR} "${GLSL_FILE}" -o ${BINARY_FILE}
			DEPENDS ${SLANG})
  		list(APPEND BINARY_FILES ${GLSL_FILE})
	else()
		set(BINARY_FILE "${SHADER_PATH}.slang-module")
		add_custom_command(
			OUTPUT ${BINARY_FILE}
			COMMAND ${SLANG_COMPILATOR} ${SLANG} -o ${BINARY_FILE}
			DEPENDS ${SLANG})
	endif()
  
  list(APPEND BINARY_FILES ${BINARY_FILE})
endforeach(SLANG)

add_custom_target(
    Shaders
    DEPENDS ${BINARY_FILES}
)
