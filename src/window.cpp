#include "window.hpp"
#include <print>

Window::Window() :
	window(nullptr)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

void Window::init(const std::string& title) {
	window = glfwCreateWindow(WIDTH_HD, HEIGHT_HD,title.c_str(), nullptr, nullptr);
}

const char** Window::getExtensions(unsigned int& extensionCount) {
	return glfwGetRequiredInstanceExtensions(&extensionCount);
}

void Window::loop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}

void Window::clean() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
void Window::render() {

	std::println("render");
}