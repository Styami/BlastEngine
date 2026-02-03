#include "window.hpp"
#include "GLFW/glfw3.h"
#include "enum_input.hpp"
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
	framebufferResized(false),
	m_buttonPressed({})
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
}

void Window::init(const std::string& title) {
	window = glfwCreateWindow(WIDTH_HD, HEIGHT_HD, title.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, cursorButtonInfoCallback);
	glfwSetKeyCallback(window, keyboardCallback);
	// glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
}

void Window::frameBufferResizeCallback(GLFWwindow* window, int, int) {
	auto renderer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	renderer->framebufferResized = true;
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
	auto renderer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	int width, height;
	renderer->getFrameBufferSize(width, height);
	renderer->m_cursorPos = {xpos, height- ypos};
}

void Window::cursorButtonInfoCallback(GLFWwindow* window, int button, int action, int) {
	auto renderer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::leftClick)] = true;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::leftClick)] = false;
	}

}

void Window::keyboardCallback(GLFWwindow* window, int key, int, int action, int) {
	auto renderer = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::forward)] = true;
	}
	if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::forward)] = false;
	}
	if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::left)] = true;
	}
	if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::left)] = false;
	}
	if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::backward)] = true;
	}
	if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::backward)] = false;
	}
	if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::right)] = true;
	}
	if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::right)] = false;
	}
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::down)] = true;
	}
	if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_RELEASE) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::down)] = false;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::up)] = true;
	}
	if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
		renderer->m_buttonPressed[static_cast<unsigned int>(Input::up)] = false;
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
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

/**
	@return relative postion of cursor from center of window. Also reverse the y axe.
*/
glm::vec2 Window::getCursorPos() const {
	double x = 0, y = 0;
	glfwGetCursorPos(window, &x, &y);
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	return {x, y};
}

std::tuple<std::array<bool, static_cast<size_t>(Input::count)>, glm::vec2> Window::getCursorInfo() const {
	return {m_buttonPressed, m_cursorPos};
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