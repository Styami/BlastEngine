#include "window.hpp"
#include <GLFW/glfw3.h>
#include <print>

Window::Window() :
	window(nullptr),
	isClosed(false),
	framebufferResized(false)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
}

void Window::init(const std::string& title) {
	window = glfwCreateWindow(WIDTH_HD, HEIGHT_HD, title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
}

void Window::frameBufferResizeCallback(GLFWwindow* window, int, int) {
	auto renderer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	renderer->framebufferResized = true;
}

bool Window::hasFrameBufferResized() const {
	return framebufferResized;
}

void Window::resetFrameBufferResized() {
	framebufferResized = false;
}

void Window::waitEvents() {
	glfwWaitEvents();
}

const char** Window::getExtensions(unsigned int& extensionCount) {
	return glfwGetRequiredInstanceExtensions(&extensionCount);
}

void Window::inputCallBack(GLFWwindow* window, int key, int, int action, int) {
	Window* renderer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if(action == GLFW_PRESS) {
		switch (key) {
			case GLFW_KEY_ESCAPE:
				renderer->isClosed = true;
				break;
			default:
				break;
		}
	}
}

bool Window::loop() {
	glfwPollEvents();
	glfwSetKeyCallback(window, inputCallBack);
	return !(isClosed || glfwWindowShouldClose(window));
}

void Window::clean() {
	glfwDestroyWindow(window);
	glfwTerminate();
}
void Window::render() {

	std::println("render");
}

void Window::createSurface(const VkInstance& instance, VkSurfaceKHR* surface) {
	if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		throw std::runtime_error("failled to create window surface.\n");
}

void Window::getFrameBufferSize(int& width, int& height) {
	glfwGetFramebufferSize(window, &width, &height);
}