cmake_minimum_required(VERSION 3.28)
project(BlastEngine VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


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

file(GLOB_RECURSE SLANG_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.slang"
)
 
foreach(SLANG ${SLANG_SOURCE_FILES})
  get_filename_component(FILE_NAME ${SLANG} NAME_WLE)
  set(BINARY_FILE "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}")
  if((${FILE_NAME} MATCHES Vert$) OR (${FILE_NAME} MATCHES Frag$))
	  	set(BINARY_FILE "${BINARY_FILE}.spv")
		add_custom_command(
  			OUTPUT ${BINARY_FILE}
			COMMAND ${SLANG_COMPILATOR} -target spirv ${SLANG} -o ${BINARY_FILE}
			DEPENDS ${SLANG})
	else()
		set(BINARY_FILE "${BINARY_FILE}.slang-module")
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
