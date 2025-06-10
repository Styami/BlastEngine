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

		struct QueueFamilyIndices {
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool is_complete() {
				return graphicsFamily.has_value() 
						&& presentFamily.has_value();
			}
		};

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

		void pickPhysicalDevice();

		int ratePhysicalDevice(const VkPhysicalDevice& device);

		QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device);

		void createLogicalDevice();

		void createSurface();

		VkDebugUtilsMessengerEXT debugMessenger;
		Window renderer;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkInstance instance;
		VkSurfaceKHR surface;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;

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