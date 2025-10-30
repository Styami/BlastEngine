#ifndef APP_HPP
#define APP_HPP

#include <vulkan/vulkan.hpp>
#include <filesystem>
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

			inline bool is_complete() {
				return graphicsFamily.has_value() 
						&& presentFamily.has_value();
			}
		};

		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;

			inline bool is_supported() {
				return !formats.empty()
						&& !presentModes.empty();
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

		bool checkDeviceExtension(const VkPhysicalDevice& device);
		
		SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device);
		
		VkSurfaceFormatKHR chooseSwapSurfaceFormats(const std::vector<VkSurfaceFormatKHR>& surfaceFormats);

		VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& surfaceFormats);

		VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		
		void createSwapChain();
		
		void createImageViews();

		std::vector<char> readFile(const std::filesystem::path& fileName);
		
		VkShaderModule createShaderModule(const std::vector<char> binaryShader);

		void createGraphicPipeline();

		void createRenderPass();

		void createFrameBuffers();

		void createCommandPool();

		void createCommandBuffer();

		void recordCommandBuffer(VkCommandBuffer& commandBuffer, uint32_t imageIndex);

		VkDebugUtilsMessengerEXT debugMessenger;
		Window renderer;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkInstance instance;
		VkSurfaceKHR surface;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSwapchainKHR swapChain;
		std::vector<VkImage> swapChainImages;
		VkFormat swapChainFormat;
		VkExtent2D swapChainExtent;
		std::vector<VkImageView> swapChainImageViews;
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		std::vector<VkFramebuffer> swapChainFrameBuffers;
		VkCommandPool commandPool;
		VkCommandBuffer commandBuffer;

		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		#ifdef NDEBUG
		const bool enableValidationLayers = false;
		#else
		const bool enableValidationLayers = true;
		#endif
};

#endif