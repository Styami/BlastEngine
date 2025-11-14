#include "app.hpp"
#include "VkBootstrap.h"
#include <stdexcept>
#include <print>
#include <iostream>
#include <fstream>
#include <format>
#include <algorithm>
#include <map>
#include <set>
#include <memory>


App::App()
{
	std::println("Construct App.");
}

void App::createInstance() {
	vkb::InstanceBuilder instanceBuilder;

	instanceBuilder.set_app_name("Blast Engine")
									.set_engine_name("Blast Engine")
									.set_engine_version(VK_MAKE_VERSION(0, 2, 0))
									.require_api_version(VK_API_VERSION_1_4);
	
	auto systemInfo_ret = vkb::SystemInfo::get_system_info();
	if (!systemInfo_ret) {
		throw std::runtime_error("Failed to get system info.");
	}
	vkb::SystemInfo systemInfo = systemInfo_ret.value();

	std::ranges::for_each(validationLayers, [&systemInfo, & instanceBuilder](const char* c) {
		if (systemInfo.is_layer_available(c)) {
			instanceBuilder.enable_validation_layers(c);
		}
	});

	std::ranges::for_each(deviceExtensions, [&systemInfo, & instanceBuilder](const char* c) {
		if (systemInfo.is_extension_available(c)) {
			instanceBuilder.enable_extension(c);
		}
	});
	instanceBuilder.use_default_debug_messenger();
	auto instanceBuildRet = instanceBuilder.build();
	if(!instanceBuildRet) {
		throw std::runtime_error("failed to create instance\n");	
	}
	vkbInstance = instanceBuildRet.value();
}

void App::getPhysicalDevice() {
	vkb::PhysicalDeviceSelector selector = vkb::PhysicalDeviceSelector(vkbInstance);

	selector.set_surface(surface)
					.prefer_gpu_device_type()
					.set_minimum_version(1, 4);
	// take in account all required extension that the device has to support
	std::ranges::for_each(deviceExtensions, [&selector](const char* c) {
		selector.add_required_extension(c);
	});
	auto selectedDevice = selector.select();
	if(!selectedDevice) {
		throw std::runtime_error("Failed to find a physical device.");
	}
	vkbPhysicalDevice = selectedDevice.value();
}

void App::createLogicalDevice() {
	vkb::DeviceBuilder builder = vkb::DeviceBuilder(vkbPhysicalDevice);
	auto builderRet = builder.build();
	if (!builderRet) {
		throw std::runtime_error("Failed to create logical device.");
	}
	vkbDevice = builderRet.value();
}

void App::getQueueFamilies() {
	auto graphicQueueRet = vkbDevice.get_queue(vkb::QueueType::graphics);
	auto presentQueueRet = vkbDevice.get_queue(vkb::QueueType::present);
	if (!graphicQueueRet || !presentQueueRet) {
		throw std::runtime_error("Failed to create all queue.\n");
	}
	graphicsQueue = graphicQueueRet.value();
	presentQueue = presentQueueRet.value();
}

void App::createSwapChain() {
	vkb::SwapchainBuilder scBuilder = vkb::SwapchainBuilder(vkbDevice);
	auto scBuilderRet = scBuilder.set_desired_min_image_count(2)
																									.build();
	if (!scBuilderRet) {
		throw std::runtime_error("Failed to create swap chain.");
	}
	vkbSwapChain = scBuilderRet.value();
}

void App::createSurface() {
	renderer.createSurface(vkbInstance.instance, &surface);
}

void App::recreateSwapChain() {
	vkDeviceWaitIdle(vkbDevice.device);

	cleanUpSwapChain();
	int width, height = 0;
	renderer.getFrameBufferSize(width, height);
	while (width == 0 || height == 0)
	{
		renderer.waitEvents();
		renderer.getFrameBufferSize(width, height);
	}
	
	createSwapChain();
	createImageViews();
	createFrameBuffers();
}

void App::createImageViews() {
	const std::vector<VkImage>& swapChainImages = vkbSwapChain.get_images().value();
	swapChainImageViews.resize(vkbSwapChain.image_count);
	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = vkbSwapChain.image_format;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(vkbDevice.device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
		    throw std::runtime_error("failed to create image views!");

	}
	
}

std::vector<char> App::readFile(const std::filesystem::path& fileName) {
	std::string test = std::filesystem::current_path().string();
	if (!std::filesystem::exists(fileName)) {
		throw (std::format("File : \"{}\" does not exist!", fileName.string()));
	}
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);
	if (!file.is_open())
	{
		throw (std::format("File : \"{}\" could not have been open!", fileName.string()));
	}
	size_t size_file = (size_t)file.tellg();
	std::vector<char>buffer(size_file);

	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), static_cast<std::streamsize>(size_file));
	file.close();

	return buffer;
}

VkShaderModule App::createShaderModule(const std::vector<char> binaryShader) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = binaryShader.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(binaryShader.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(vkbDevice.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void App::createGraphicPipeline() {
	std::vector<char> shader = readFile("shaders/firstShader.spv");
	//std::vector<char> shaderFrag = readFile("shaders/firstShaderFrag.spv");

	VkShaderModule shaderModule = createShaderModule(shader);
	//VkShaderModule shaderFragModule = createShaderModule(shaderFrag);

	VkPipelineShaderStageCreateInfo shaderVertCreateInfo{};
	shaderVertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderVertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderVertCreateInfo.module = shaderModule;
	shaderVertCreateInfo.pName = "vertexMain";

	VkPipelineShaderStageCreateInfo shaderFragCreateInfo{};
	shaderFragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderFragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderFragCreateInfo.module = shaderModule;
	shaderFragCreateInfo.pName = "fragmentMain";

	VkPipelineShaderStageCreateInfo shaderStages[] = {shaderVertCreateInfo, shaderFragCreateInfo};

	
	VkPipelineVertexInputStateCreateInfo vertexInputInfos {};
	vertexInputInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfos.vertexAttributeDescriptionCount = 0;
	vertexInputInfos.pVertexAttributeDescriptions = nullptr;
	vertexInputInfos.vertexBindingDescriptionCount = 0;
	vertexInputInfos.pVertexBindingDescriptions = nullptr;
	
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};
	
	VkPipelineDynamicStateCreateInfo dynamicStateInfos {};
	dynamicStateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfos.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicStateInfos.pDynamicStates = dynamicStates.data();
	
	VkPipelineViewportStateCreateInfo viewportStateInfos {};
	viewportStateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateInfos.scissorCount = 1;
	viewportStateInfos.viewportCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizationStateInfos {};
	rasterizationStateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationStateInfos.depthClampEnable = VK_FALSE;
	rasterizationStateInfos.rasterizerDiscardEnable = VK_FALSE;
	rasterizationStateInfos.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationStateInfos.lineWidth = 1;
	
	rasterizationStateInfos.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizationStateInfos.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	
	rasterizationStateInfos.depthBiasEnable = VK_FALSE;
	rasterizationStateInfos.depthBiasConstantFactor = 0;
	rasterizationStateInfos.depthBiasClamp = 0;
	rasterizationStateInfos.depthBiasSlopeFactor = 0;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachmentState {};
	colorBlendAttachmentState.colorWriteMask = 
		VK_COLOR_COMPONENT_R_BIT
		| VK_COLOR_COMPONENT_G_BIT
		| VK_COLOR_COMPONENT_B_BIT
		| VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachmentState.blendEnable = VK_TRUE;
	colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo blendStateCreateInfo {};
	blendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendStateCreateInfo.logicOpEnable = VK_FALSE;
	blendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	blendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
	blendStateCreateInfo.attachmentCount = 1;
	blendStateCreateInfo.blendConstants[0] = 0;
	blendStateCreateInfo.blendConstants[1] = 0;
	blendStateCreateInfo.blendConstants[2] = 0;
	blendStateCreateInfo.blendConstants[3] = 0;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	if(vkCreatePipelineLayout(vkbDevice.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo graphicPipelineInfo{};
	graphicPipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	graphicPipelineInfo.stageCount = 2;
	graphicPipelineInfo.pStages = shaderStages;

	graphicPipelineInfo.pVertexInputState = &vertexInputInfos;
	graphicPipelineInfo.pInputAssemblyState = &inputAssembly;
	graphicPipelineInfo.pViewportState = &viewportStateInfos;
	graphicPipelineInfo.pRasterizationState = &rasterizationStateInfos;
	graphicPipelineInfo.pMultisampleState = & multisampling;
	graphicPipelineInfo.pDepthStencilState = nullptr;
	graphicPipelineInfo.pColorBlendState = &blendStateCreateInfo;
	graphicPipelineInfo.pDynamicState = &dynamicStateInfos;

	graphicPipelineInfo.layout = pipelineLayout;

	graphicPipelineInfo.renderPass = renderPass;
	graphicPipelineInfo.subpass = 0;

	graphicPipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	graphicPipelineInfo.basePipelineIndex = -1;

	if(vkCreateGraphicsPipelines(vkbDevice.device, VK_NULL_HANDLE, 1, &graphicPipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!!");
	}

	//vkDestroyShaderModule(vkbDevice.device, shaderVertModule, nullptr);
  vkDestroyShaderModule(vkbDevice.device, shaderModule, nullptr);
}

void App::createRenderPass() {
	VkAttachmentDescription colorAttchement {};
	colorAttchement.format = vkbSwapChain.image_format;
	colorAttchement.samples = VK_SAMPLE_COUNT_1_BIT;

	colorAttchement.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttchement.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	colorAttchement.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttchement.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	colorAttchement.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttchement.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference attachmentRef {};
	attachmentRef.attachment = 0;
	attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDesc {};
	subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDesc.colorAttachmentCount = 1;
	subpassDesc.pColorAttachments = &attachmentRef;

	VkSubpassDependency subpassDependency {};
	subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependency.dstSubpass = 0;
	subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.srcAccessMask = 0;
	subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttchement;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDesc;
	renderPassInfo.pDependencies = &subpassDependency;
	renderPassInfo.dependencyCount = 1;

	if(vkCreateRenderPass(vkbDevice.device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!!!");
	}

}


void App::createFrameBuffers() {
	swapChainFrameBuffers.resize(swapChainImageViews.size());

	for(size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachements[] = {
			swapChainImageViews[i]
		};
		
		VkFramebufferCreateInfo framebufferCreateInfo{};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass;
		framebufferCreateInfo.attachmentCount = 1;
		framebufferCreateInfo.pAttachments = attachements;
		framebufferCreateInfo.height = vkbSwapChain.extent.height;
		framebufferCreateInfo.width = vkbSwapChain.extent.width;
		framebufferCreateInfo.layers = 1;

		if(vkCreateFramebuffer(vkbDevice.device, &framebufferCreateInfo, nullptr, &swapChainFrameBuffers[i])
			!= VK_SUCCESS
		) {
			throw std::runtime_error("Failed to create framebuffer");
		}
	}

}

void App::createCommandPool() {
	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	if(vkCreateCommandPool(vkbDevice.device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::runtime_error("Failed to create command pool!");
}

void App::createCommandBuffers() {
	VkCommandBufferAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = commandBuffers.size();

	if(vkAllocateCommandBuffers(vkbDevice.device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
	
}

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // optional
	beginInfo.pInheritanceInfo = nullptr; // optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin the record of command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFrameBuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = vkbSwapChain.extent;
	
	VkClearValue clearColor = {{{0.01, 0.01, 0.01, 1.0}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.height = vkbSwapChain.extent.height;
	viewport.width = vkbSwapChain.extent.width;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = vkbSwapChain.extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to end command buffer.");
	}
}

void App::createSyncObjects() {
	VkSemaphoreCreateInfo semaphoreInfo {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(vkbDevice.device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
    vkCreateSemaphore(vkbDevice.device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
    vkCreateFence(vkbDevice.device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores");
		}
	}

}

void App::drawFrame() {
	// Setup fence
	vkWaitForFences(vkbDevice.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	
	// get image of swapchain and check if the swap chain is still OK
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(vkbDevice.device, vkbSwapChain.swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire image of the swapchain.");
	}
	vkResetFences(vkbDevice.device, 1, &inFlightFences[currentFrame]);
	
	
	// Setup record of command buffer
	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
	// Submitting command buffer
	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	std::array<VkSemaphore, 1> waitSemaphores = {imageAvailableSemaphores[currentFrame]};
	std::array<VkPipelineStageFlags, 1> waitStage = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.waitSemaphoreCount = waitSemaphores.size();
	submitInfo.pWaitDstStageMask = waitStage.data();
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
	submitInfo.commandBufferCount = 1;
	std::array<VkSemaphore, 1> signaledSemaphores = {renderFinishedSemaphores[currentFrame]};
	submitInfo.pSignalSemaphores = signaledSemaphores.data();
	submitInfo.signalSemaphoreCount = signaledSemaphores.size();

	if(vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit queue.");
	}
	// Present image phase
	VkPresentInfoKHR presentInfo {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pWaitSemaphores = signaledSemaphores.data();
	presentInfo.waitSemaphoreCount = 1;
	std::array<VkSwapchainKHR, 1> swapChains = {vkbSwapChain.swapchain};
	presentInfo.pSwapchains = swapChains.data();
	presentInfo.swapchainCount = swapChains.size();
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	
	result = vkQueuePresentKHR(presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderer.hasFrameBufferResized())
	{
		recreateSwapChain();
		renderer.resetFrameBufferResized();
	} else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present image.");
	}
	
	
	currentFrame = (currentFrame + 1) % MAX_FRAME_IN_FLIGHT;
}

void App::initVulkan() {
	renderer.init("Blast Engine");
	createInstance();
	createSurface();
	getPhysicalDevice();
	createLogicalDevice();
	getQueueFamilies();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicPipeline();
	createFrameBuffers();
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();
}


void App::mainLoop() {
	while (appRunning) {
		appRunning = !renderer.loop();
		drawFrame();
	}
	vkDeviceWaitIdle(vkbDevice.device);
}

void App::cleanUpSwapChain() {
	
	// vkDestroySwapchainKHR(vkbDevice.device, vkbSwapChain.swapChain, nullptr);
	vkb::destroy_swapchain(vkbSwapChain);
	for (VkFramebuffer framebuffer : swapChainFrameBuffers) {
		vkDestroyFramebuffer(vkbDevice.device, framebuffer, nullptr);
	}
	for(VkImageView& imageView : swapChainImageViews) {
		vkDestroyImageView(vkbDevice.device, imageView, nullptr);
	}
}

void App::cleanUp() {
	// if (enableValidationLayers) {
	// 	DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	// }
	cleanUpSwapChain();
	//vkb::destroy_swapchain(vkbSwapChain);
	vkDestroyPipelineLayout(vkbDevice.device, pipelineLayout, nullptr);
	vkDestroyRenderPass(vkbDevice.device, renderPass, nullptr);
	vkDestroyPipeline(vkbDevice.device, graphicsPipeline, nullptr);
	vkDestroyCommandPool(vkbDevice.device, commandPool, nullptr);
	for(size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) { 
		vkDestroySemaphore(vkbDevice.device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(vkbDevice.device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(vkbDevice.device, inFlightFences[i], nullptr);
	}
	// vkDestroyDevice(device, nullptr);
	vkb::destroy_device(vkbDevice);
	vkDestroySurfaceKHR(vkbInstance.instance, surface, nullptr);
	vkb::destroy_instance(vkbInstance);
	// vkDestroyInstance(instance, nullptr);
	renderer.clean();
	std::println("clean");
}

void App::run() {
	try
	{
		initVulkan();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return;
	}
	std::println("test\n");
	mainLoop();
	cleanUp();
	std::println("Test run.");
}