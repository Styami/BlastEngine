cmake_minimum_required(VERSION 3.28)
project(BlastEngine VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

add_executable(BlastEngine)

if(MSVC)
	target_compile_options(BlastEngine PRIVATE /W4 /WX)
elseif(MINGW)
	target_compile_options(BlastEngine PRIVATE -Wall -Wextra -Wpedantic -Werror -g)
	target_link_libraries(BlastEngine PRIVATE -lstdc++exp)
else()
  target_compile_options(BlastEngine PRIVATE -Wall -Wextra -Wpedantic -Werror -g -O0)
endif()

find_package(Vulkan REQUIRED)

set(SLANG_ENABLE_VULKAN ON)
set(SLANG_USE_SYSTEM_VULKAN_HEADERS ON)
set(SLANG_ENABLE_TESTS OFF)
set(SLANG_ENABLE_EXAMPLES OFF)
set(SLANG_ENABLE_GFX OFF)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(ext/glfw)
add_subdirectory(ext/glm)
add_subdirectory(ext/slang)
add_subdirectory(ext/vk-bootstrap)
add_subdirectory(ext/tinyobjloader)

target_include_directories(BlastEngine PRIVATE
	include
	ext/stb
)
target_link_libraries(BlastEngine PRIVATE 
	glfw
	Vulkan::Vulkan
	vk-bootstrap::vk-bootstrap
	slang
	glm::glm
	tinyobjloader
)