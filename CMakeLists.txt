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
#based on this CMAKE

find_program(SLANG_COMPILATOR slangc)

file(GLOB_RECURSE SLANG_SOURCE_FILES
  "${PROJECT_SOURCE_DIR}/shaders/*.slang"
)
 
foreach(SLANG ${SLANG_SOURCE_FILES})
  get_filename_component(FILE_NAME ${SLANG} NAME_WLE)
  set(SPIRV "${PROJECT_SOURCE_DIR}/shaders/${FILE_NAME}.spv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${SLANG_COMPILATOR} -target spirv ${SLANG} -o ${SPIRV}
    DEPENDS ${SLANG})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(SLANG)
 
add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)
