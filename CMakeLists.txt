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
  target_compile_options(BlastEngine PRIVATE -Wall -Wextra -Wpedantic -Werror -g)
endif()

if(WIN32)
add_subdirectory("C:/glfw-3.4/" glfw3)
elseif(UNIX)
find_package(glfw3 3.4 REQUIRED)
endif()

find_package(Vulkan REQUIRED)
find_package(slang REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(ext/vk-bootstrap)

target_include_directories(BlastEngine PRIVATE
	include
	ext/stb
)
target_link_libraries(BlastEngine PRIVATE 
	glfw
	Vulkan::Vulkan
	vk-bootstrap::vk-bootstrap
	slang::slang
	glm::glm
)