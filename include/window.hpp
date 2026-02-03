#ifndef WINDOW_HPP
#define WINDOW_HPP
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>
#include "enum_input.hpp"

const uint16_t WIDTH_HD = 1280;
const uint16_t HEIGHT_HD = 720;
const uint16_t WIDTH_FHD = 1920;
const uint16_t HEIGHT_FHD = 1090;
const uint16_t WIDTH_2K = 2560;
const uint16_t HEIGHT_2K = 1440;

class Window {
	public:
		Window(const Window& another);
		Window(Window&& another);
		Window& operator=(const Window& another);
		Window& operator=(Window&& another);
		bool operator==(const Window& another);
		bool operator!=(const Window& another);
		Window();
		void init(const std::string& title);
		bool loop();
		void clean();
		const char** getExtensions(unsigned int& extensionCount);
		void render();
		void createSurface(const VkInstance& instance, VkSurfaceKHR* surface);
		void getFrameBufferSize(int& width, int &height);
		bool hasFrameBufferResized() const;
		void resetFrameBufferResized();
		void waitEvents();
		GLFWwindow* getWindow() const;
		glm::vec2 getCursorPos() const;
		std::tuple<std::array<bool, static_cast<size_t>(Input::count)>, glm::vec2> getCursorInfo() const;
		
	private:
		static void frameBufferResizeCallback(GLFWwindow* window, int width, int height);
		static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
		static void cursorButtonInfoCallback(GLFWwindow* window, int button, int action, int mods);
		static void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		GLFWwindow* window;
		bool framebufferResized;
		glm::vec2 m_cursorPos;
		std::array<bool, static_cast<size_t>(Input::count)> m_buttonPressed;
};

#endif