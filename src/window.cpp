#include "window.hpp"
#include <cstddef>
#include <print>

Window::Window(const Window& another) :
	window(another.window),
	framebufferResized(another.framebufferResized)
{}

Window::Window(Window&& another) :
	window(another.window),
	framebufferResized(another.framebufferResized)
{
	if (another != *this) {
		glfwDestroyWindow(window);
		window = another.window;
		another.window = nullptr;
		framebufferResized = another.framebufferResized;
		another.framebufferResized = false;
	}

}

bool Window::operator==(const Window& another) {
	return window == another.window && framebufferResized == another.framebufferResized;
}

bool Window::operator!=(const Window& another) {
	return !(*this == another);
}

Window& Window::operator=(const Window& another) {
	window = another.window;
	framebufferResized = another.framebufferResized;

	return *this;
}

Window& Window::operator=(Window&& another) {
	window = std::move(another.window);
	framebufferResized = std::move(another.framebufferResized);

	return *this;
}

Window::Window() :
	window(nullptr),
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


bool Window::loop() {
	glfwPollEvents();
	return !glfwWindowShouldClose(window);
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

GLFWwindow* Window::getWindow() const {
	return window;
}