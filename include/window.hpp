#ifndef WINDOW_HPP
#define WINDOW_HPP
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

const uint16_t WIDTH_HD = 1280;
const uint16_t HEIGHT_HD = 720;
const uint16_t WIDTH_FHD = 1920;
const uint16_t HEIGHT_FHD = 1090;
const uint16_t WIDTH_2K = 2560;
const uint16_t HEIGHT_2K = 1440;

class Window {
	public:
		Window();
		void init(const std::string& title);
		int loop();
		void clean();
		const char** getExtensions(unsigned int& extensionCount);
		void render();
		void createSurface(const VkInstance& instance, VkSurfaceKHR* surface);
		void getFrameBuffer(int& width, int &height);
	
	private:
		GLFWwindow* window;
};

#endif