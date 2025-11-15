#ifndef APP_HPP
#define APP_HPP

#include <vulkan/vulkan.hpp>
#include <filesystem>
#include "VkBootstrap.h"
#include "window.hpp"
#include <vector>

const int MAX_FRAME_IN_FLIGHT = 2;
class Engine
{
	public:

		Engine();

		void initVulkan();

		void drawFrame();

		void run();

	private:

		void createInstance();

		void mainLoop();

		void cleanUp();

		void getPhysicalDevice();

		void getQueueFamilies();

		void createLogicalDevice();

		void createSurface();
		
		void createSwapChain();
		
		void recreateSwapChain();

		void cleanUpSwapChain();
		
		void createImageViews();

		std::vector<char> readFile(const std::filesystem::path& fileName);
		
		VkShaderModule createShaderModule(const std::vector<char> binaryShader);

		void createGraphicPipeline();

		void createRenderPass();

		void createFrameBuffers();

		void createCommandPool();

		void createCommandBuffers();

		void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		void createSyncObjects();
		
		vkb::Instance vkbInstance;
		vkb::PhysicalDevice vkbPhysicalDevice;
		vkb::Device vkbDevice;
		vkb::Swapchain vkbSwapChain;
		Window renderer;
		VkSurfaceKHR surface;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		std::vector<VkImage> swapChainImages;
		std::vector<VkImageView> swapChainImageViews;
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		std::vector<VkFramebuffer> swapChainFrameBuffers;
		VkCommandPool commandPool;
		std::array<VkCommandBuffer, MAX_FRAME_IN_FLIGHT> commandBuffers;
		std::array<VkSemaphore, MAX_FRAME_IN_FLIGHT> imageAvailableSemaphores;
		std::array<VkSemaphore, MAX_FRAME_IN_FLIGHT> renderFinishedSemaphores;
		std::array<VkFence, MAX_FRAME_IN_FLIGHT> inFlightFences;

		bool engineRunning = true;
		uint32_t currentFrame = 0;
		
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_SHADER_DRAW_PARAMETERS_EXTENSION_NAME
		};

		#ifdef NDEBUG
		const bool enableValidationLayers = false;
		#else
		const bool enableValidationLayers = true;
		#endif
};

#endif