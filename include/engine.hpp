#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <tuple>
#include <vulkan/vulkan.hpp>
#include <filesystem>
#include "VkBootstrap.h"
#include "window.hpp"
#include "object.hpp"
#include "buffer.hpp"

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

		vk::ShaderModule createShaderModule(const std::string& binaryShader);

		void createGraphicPipeline();

		void createCommandPool();

		void createCommandBuffers();

		void transition_image_layout(
			vk::CommandBuffer commandBuffer,
			uint32_t imageIndex,
			vk::ImageLayout oldLayout,
			vk::ImageLayout newLayout,
			vk::AccessFlags2 srcAccessMask,
			vk::AccessFlags2 dstAccessMask,
			vk::PipelineStageFlags2 srcStageMask,
			vk::PipelineStageFlags2 dstStageMask
		);

		void recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex);

		void createSyncObjects();

		void createVertexBuffer();

		void loadObjects();

		vkb::Instance vkbInstance;
		vk::Instance vkInstance;
		vkb::PhysicalDevice vkbPhysicalDevice;
		vk::PhysicalDevice vkPhysicalDevice;
		vkb::Device vkbDevice;
		vk::Device vkDevice;
		vkb::Swapchain vkbSwapChain;
		vk::SwapchainKHR vkSwapChain;
		vk::Extent2D swapChainExtent;
		vk::Format swapChainImageFormat;
		Window renderer;
		vk::SurfaceKHR surface;
		vk::Queue graphicsQueue;
		vk::Queue presentQueue;
		std::vector<vk::Image> swapChainImages;
		std::vector<vk::ImageView> swapChainImageViews;
		vk::PipelineLayout pipelineLayout;
		vk::Pipeline graphicsPipeline;
		std::vector<VkFramebuffer> swapChainFrameBuffers;
		vk::CommandPool commandPool;
		std::array<vk::CommandBuffer, MAX_FRAME_IN_FLIGHT> commandBuffers;
		std::array<vk::Semaphore, MAX_FRAME_IN_FLIGHT> imageAvailableSemaphores;
		std::array<vk::Semaphore, MAX_FRAME_IN_FLIGHT> renderFinishedSemaphores;
		std::array<vk::Fence, MAX_FRAME_IN_FLIGHT> inFlightFences;
		std::vector<Object> objects;
		be::Buffer vbo;
		

		bool engineRunning = true;
		uint32_t currentFrame = 0;

		const std::vector<const char*> deviceExtensions = {
			vk::KHRSwapchainExtensionName,
			vk::KHRSpirv14ExtensionName,
			vk::KHRSynchronization2ExtensionName,
			vk::KHRCreateRenderpass2ExtensionName
		};
};

#endif