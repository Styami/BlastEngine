#ifndef APP_HPP
#define APP_HPP

#include <vulkan/vulkan.hpp>
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
		std::vector<const char*> getRequiredExtension();
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, 
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData
		);
		void setupDebugMessenger();
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
											const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
											const VkAllocationCallbacks* pAllocator,
											VkDebugUtilsMessengerEXT* pDebugMessenger);
		
		void DestroyDebugUtilsMessengerEXT(VkInstance instance,
											VkDebugUtilsMessengerEXT debugMessenger,
											const VkAllocationCallbacks* pAllocator);

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkDebugUtilsMessengerEXT debugMessenger;
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