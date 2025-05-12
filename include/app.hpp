#ifndef APP_HPP
#define APP_HPP

#include <vulkan/vulkan.h>
#include "window.hpp"
#include <vector>

class App
{
	public:
		App();
		void run();


	private:
		void initVulkan();
		void createInstance();
		void mainLoop();
		void cleanUp();
		bool checkExtension();
		bool checkValidationLayerSupport();

		Window renderer;
		VkInstance instance;
		
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		#ifdef NDEBUG
		const bool enableValidationLayers = false;
		#else
		const bool enableValidationLayers = true;
		#endif
};

#endif