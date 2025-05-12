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
