#include "engine.hpp"
#include "buffer.hpp"
#include "shaderCompiler.hpp"
#include <ranges>
#include <print>


Engine::Engine()
{
	std::println("Construct Engine.");
}

void Engine::createInstance() {
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

	std::ranges::for_each(deviceExtensions, [&systemInfo, & instanceBuilder](const char* c) {
		if (systemInfo.is_extension_available(c)) {
			instanceBuilder.enable_extension(c);
		}
	});

	auto instanceBuildRet = instanceBuilder.build();
	if(!instanceBuildRet) {
		throw std::runtime_error("failed to create instance\n");	
	}

	vkbInstance = instanceBuildRet.value();
	vkInstance = vkbInstance.instance;
}

void Engine::getPhysicalDevice() {
	vkb::PhysicalDeviceSelector selector = vkb::PhysicalDeviceSelector(vkbInstance);

	selector.set_surface(surface)
					.prefer_gpu_device_type()
					.set_minimum_version(1, 4);

	vk::PhysicalDeviceVulkan13Features features13 = vk::PhysicalDeviceVulkan13Features()
													.setDynamicRendering(vk::True)
													.setSynchronization2(vk::True);

	// take in account all required extension that the device has to support
	std::ranges::for_each(deviceExtensions, [&selector](const char* c) {
		selector.add_required_extension(c);
	});
	auto selectedDevice = selector.set_required_features_13(features13)
													.select();
	if(!selectedDevice) {
		throw std::runtime_error("Failed to find a physical device.");
	}
	vkbPhysicalDevice = selectedDevice.value();
	vkPhysicalDevice = vkbPhysicalDevice.physical_device;
}

void Engine::createLogicalDevice() {
	vkb::DeviceBuilder builder = vkb::DeviceBuilder(vkbPhysicalDevice);
	auto builderRet = builder.build();
	if (!builderRet) {
		throw std::runtime_error("Failed to create logical device.");
	}
	vkbDevice = builderRet.value();
	vkDevice = vkbDevice.device;
}

void Engine::getQueueFamilies() {
	auto graphicQueueRet = vkbDevice.get_queue(vkb::QueueType::graphics);
	auto presentQueueRet = vkbDevice.get_queue(vkb::QueueType::present);
	if (!graphicQueueRet || !presentQueueRet) {
		throw std::runtime_error("Failed to create all queue.\n");
	}
	graphicsQueue = graphicQueueRet.value();
	presentQueue = presentQueueRet.value();
}

void Engine::createSwapChain() {
	vkb::SwapchainBuilder scBuilder = vkb::SwapchainBuilder(vkbDevice);
	auto scBuilderRet = scBuilder.set_desired_min_image_count(2)
													.build();
	if (!scBuilderRet) {
		throw std::runtime_error("Failed to create swap chain.");
	}
	vkbSwapChain = scBuilderRet.value();
	vkSwapChain = vkbSwapChain.swapchain;
	swapChainExtent = vkbSwapChain.extent;
	swapChainImageFormat = vk::Format(vkbSwapChain.image_format);
	swapChainImages = vkbSwapChain.get_images().value() | std::views::transform([](const VkImage& image) {
		return vk::Image(image);
	}) | std::ranges::to<std::vector>();
}

void Engine::createSurface() {
	VkSurfaceKHR _surface;
	renderer.createSurface(vkInstance, &_surface);
	surface = vk::SurfaceKHR(_surface);
}

void Engine::recreateSwapChain() {
	vkDevice.waitIdle();

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
}

void Engine::createImageViews() {
	swapChainImageViews = vkbSwapChain.get_image_views().value() | std::views::transform([](const VkImageView& image){
		return vk::ImageView(image);
	}) | std::ranges::to<std::vector>(); 
}

vk::ShaderModule Engine::createShaderModule(const std::string& binaryShader) {
	vk::ShaderModuleCreateInfo createInfo = vk::ShaderModuleCreateInfo(
		{},
		binaryShader.size(),
		reinterpret_cast<const uint32_t*>(binaryShader.c_str())
	);
	
	return vkDevice.createShaderModule(createInfo);
}

void Engine::createGraphicPipeline() {
	ShaderCompiler compiler;
	compiler.createSession(SLANG_SPIRV, "spirv_1_5");
	std::string shader = compiler.loadProgram("firstShader");
	vk::ShaderModule shaderModule = createShaderModule(shader);

	vk::PipelineShaderStageCreateInfo shaderVertCreateInfo = vk::PipelineShaderStageCreateInfo(
		{},
		vk::ShaderStageFlagBits::eVertex,
		shaderModule,
		"vertexMain"
	);
	vk::PipelineShaderStageCreateInfo shaderFragCreateInfo = vk::PipelineShaderStageCreateInfo(
		{},
		vk::ShaderStageFlagBits::eFragment,
		shaderModule,
		"fragmentMain"
	);
	std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages = {shaderVertCreateInfo, shaderFragCreateInfo};


	auto vertexBindingDesc = Vertex::getBindingDescription();
	auto vertexAttriDesc = Vertex::getAttributeDescriptions(); 
	
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo(
		{}, 
		1,
		&vertexBindingDesc,
		vertexAttriDesc.size(),
		vertexAttriDesc.data()
	);
	
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = vk::PipelineInputAssemblyStateCreateInfo(
		{}, 
		vk::PrimitiveTopology::eTriangleList
	);
	
	std::vector<vk::DynamicState> dynamicStates = {
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};
	
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo = vk::PipelineDynamicStateCreateInfo(
		{}, 
		dynamicStates.size(),
		dynamicStates.data()
	);
	
	vk::PipelineViewportStateCreateInfo viewportStateInfo = vk::PipelineViewportStateCreateInfo(
		{},
		1,
		{},
		1
	);

	vk::PipelineRasterizationStateCreateInfo rasterizationStateInfo = vk::PipelineRasterizationStateCreateInfo(
		{},
		vk::False,
		vk::False,
		vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eBack,
		vk::FrontFace::eCounterClockwise,
		vk::False
	).setLineWidth(1);
	

	vk::PipelineMultisampleStateCreateInfo multisamplingStateInfo = vk::PipelineMultisampleStateCreateInfo(
		{},
		vk::SampleCountFlagBits::e1,
		vk::False
	);

	vk::PipelineColorBlendAttachmentState colorBlendAttachmentState;
	colorBlendAttachmentState.setColorWriteMask(
		vk::ColorComponentFlagBits::eR
		| vk::ColorComponentFlagBits::eG
		| vk::ColorComponentFlagBits::eB
		| vk::ColorComponentFlagBits::eA
	);
	colorBlendAttachmentState.setBlendEnable(vk::False);
	colorBlendAttachmentState.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha);
	colorBlendAttachmentState.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha);
	colorBlendAttachmentState.setColorBlendOp(vk::BlendOp::eAdd);
	colorBlendAttachmentState.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
	colorBlendAttachmentState.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
	colorBlendAttachmentState.setAlphaBlendOp(vk::BlendOp::eAdd);

	vk::PipelineColorBlendStateCreateInfo blendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo(
		{},
		vk::False,
		vk::LogicOp::eCopy,
		1,
		&colorBlendAttachmentState
	);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo = vk::PipelineLayoutCreateInfo(
		{},
		0,
		{},
		0
	);
	
	pipelineLayout = vkDevice.createPipelineLayout(pipelineLayoutInfo);
	
	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo(
		{},
		1,
		&swapChainImageFormat
	);

	vk::GraphicsPipelineCreateInfo graphicPipelineInfo = vk::GraphicsPipelineCreateInfo(
		{},
		shaderStages.size(),
		shaderStages.data(),
		&vertexInputInfo,
		&inputAssemblyInfo,
		{},
		&viewportStateInfo,
		&rasterizationStateInfo,
		&multisamplingStateInfo,
		{},
		&blendStateCreateInfo,
		&dynamicStateInfo,
		pipelineLayout
	);
	graphicPipelineInfo.setPNext(&pipelineRenderingCreateInfo);

	auto res = vkDevice.createGraphicsPipeline({},graphicPipelineInfo);
	if(res.result != vk::Result::eSuccess) {
		throw std::runtime_error("failed to create graphics pipeline!!");
	}
	
	graphicsPipeline = res.value;
	vkDestroyShaderModule(vkbDevice.device, shaderModule, nullptr);
}


void Engine::loadObjects() {
	objects.push_back(Object());
}

void Engine::createCommandPool() {
	vk::CommandPoolCreateInfo commandPoolCreateInfo = vk::CommandPoolCreateInfo(
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		vkbDevice.get_queue_index(vkb::QueueType::graphics).value()
	);

	commandPool = vkDevice.createCommandPool(commandPoolCreateInfo);
}

void Engine::createVertexBuffer() {
	loadObjects(); // temporary. IT HAS TO BE CHANGE IN FUTURE!!!!!!!
	std::vector<Vertex> verticesToRender;
	for (Object object : objects) {
		std::ranges::for_each(object.getVertices(), [&verticesToRender](const Vertex& v) {
			verticesToRender.push_back(v);
		});
	}
	vk::DeviceSize vboSize = sizeof(Vertex) * verticesToRender.size();
	be::Buffer stagingBuffer = be::Buffer(vkDevice, vboSize);
	stagingBuffer.create(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, vkPhysicalDevice);
	stagingBuffer.map(verticesToRender);

	vbo = be::Buffer(vkDevice, vboSize);
	vbo.create(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::SharingMode::eExclusive, vkPhysicalDevice);
	vbo.copyBuffer(stagingBuffer, commandPool, graphicsQueue);


}

void Engine::createIndexBuffer() {
	std::vector<int> indicesOfVertices;
	for (Object object : objects) {
		std::ranges::for_each(object.getIndices(), [&indicesOfVertices](const int i) {
			indicesOfVertices.push_back(i);
		});
	}
	vk::DeviceSize iboSize = sizeof(int) * indicesOfVertices.size();
	be::Buffer stagingBuffer = be::Buffer(vkDevice, iboSize);
	stagingBuffer.create(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, vkPhysicalDevice);
	stagingBuffer.map(indicesOfVertices);

	ibo = be::Buffer(vkDevice, iboSize);
	ibo.create(vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::SharingMode::eExclusive, vkPhysicalDevice);
	ibo.copyBuffer(stagingBuffer, commandPool, graphicsQueue);
}

void Engine::createCommandBuffers() {
	vk::CommandBufferAllocateInfo allocateInfo = vk::CommandBufferAllocateInfo(
		commandPool,
		vk::CommandBufferLevel::ePrimary,
		commandBuffers.size()
	);

	std::copy_n(vkDevice.allocateCommandBuffers(allocateInfo).begin(), commandBuffers.size(), commandBuffers.begin());
}

void Engine::transition_image_layout(
	vk::CommandBuffer commandBuffer,
	uint32_t imageIndex,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout,
	vk::AccessFlags2 srcAccessMask,
	vk::AccessFlags2 dstAccessMask,
	vk::PipelineStageFlags2 srcStageMask,
	vk::PipelineStageFlags2 dstStageMask
) {
	vk::ImageMemoryBarrier2 barrier(
		srcStageMask,
		srcAccessMask,
		dstStageMask,
		dstAccessMask,
		oldLayout,
		newLayout,
		VK_QUEUE_FAMILY_IGNORED,
		VK_QUEUE_FAMILY_IGNORED,
		swapChainImages[imageIndex],
		vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
	);

	vk::DependencyInfo dependencyInfo = vk::DependencyInfo({}, {}, {}, {}, {}, 1, &barrier);
	commandBuffer.pipelineBarrier2(dependencyInfo);
}

void Engine::recordCommandBuffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex) {	
	vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
	commandBuffer.begin(beginInfo);

	transition_image_layout(
		commandBuffer,
		imageIndex,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput
	);
	vk::ClearValue clearColor = vk::ClearColorValue(0.01f, 0.01f, 0.01f, 1.0f);
	vk::RenderingAttachmentInfo attachementInfo =
	vk::RenderingAttachmentInfo(
		swapChainImageViews[imageIndex],
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ResolveModeFlagBits::eNone,
		{},
		vk::ImageLayout::eUndefined,
		vk::AttachmentLoadOp::eClear,
		vk::AttachmentStoreOp::eStore,
		clearColor
	);
	vk::RenderingInfo renderingInfo = vk::RenderingInfo(
		{},
		vk::Rect2D({0,0}, swapChainExtent),
		1,
		{},
		1,
		&attachementInfo
	);
	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
	VkDeviceSize offests[] = {0};
	commandBuffer.bindVertexBuffers(0, 1, &vbo.getBuffer(), offests);
	commandBuffer.bindIndexBuffer(ibo.getBuffer(), 0, vk::IndexType::eUint32);

	vk::Viewport viewport = vk::Viewport(
		0,
		0,
		swapChainExtent.width,
		swapChainExtent.height,
		0,
		1
	);
	commandBuffer.setViewport(0, 1, &viewport);

	vk::Rect2D scissor = vk::Rect2D(
		{0, 0},
		swapChainExtent
	);
	commandBuffer.setScissor(0, 1, &scissor);

	commandBuffer.drawIndexed(6, 1, 0, 0, 0);
	commandBuffer.endRendering();
	transition_image_layout(
		commandBuffer,
		imageIndex,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		{},
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eBottomOfPipe
	);
	
	commandBuffer.end();
}

void Engine::createSyncObjects() {
	for(size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
		imageAvailableSemaphores[i] = vkDevice.createSemaphore(vk::SemaphoreCreateInfo());
		renderFinishedSemaphores[i] = vkDevice.createSemaphore(vk::SemaphoreCreateInfo());
		inFlightFences[i] = vkDevice.createFence(vk::FenceCreateInfo(vk::FenceCreateFlagBits::eSignaled));
	}
}

void Engine::drawFrame() {
	// Setup fence
	while(vkDevice.waitForFences(1, &inFlightFences[currentFrame], vk::True, UINT64_MAX) == vk::Result::eTimeout)
		;
	
	// get image of swapchain and check if the swap chain is still OK
	uint32_t imageIndex;
	VkResult AcquiredResult = vkAcquireNextImageKHR(vkDevice,
		vkSwapChain,
		UINT64_MAX,
		imageAvailableSemaphores[currentFrame],
		{}, 
		&imageIndex
	);
	if (AcquiredResult == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	} else if (AcquiredResult != VK_SUCCESS && AcquiredResult != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to acquire image of the swapchain.");
	}
	vk::Result vkResult = vkDevice.resetFences(1, &inFlightFences[currentFrame]);
	
	
	// Setup record of command buffer
	commandBuffers[currentFrame].reset();
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	// Submitting command buffer
	vk::SubmitInfo submitInfo = vk::SubmitInfo(
		1,
		&imageAvailableSemaphores[currentFrame],
		&waitDstStageMask,
		1,
		&commandBuffers[currentFrame],
		1,
		&renderFinishedSemaphores[currentFrame]
	);

	graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

	// Present image phase
	vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR(
		1,
		&renderFinishedSemaphores[currentFrame],
		1,
		&vkSwapChain,
		&imageIndex
	);
	
	vkResult = presentQueue.presentKHR(presentInfo);
	if (vkResult == vk::Result::eErrorOutOfDateKHR || vkResult == vk::Result::eSuboptimalKHR || renderer.hasFrameBufferResized())
	{
		recreateSwapChain();
		renderer.resetFrameBufferResized();
	} else if (vkResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to present image.");
	}
	
	
	currentFrame = (currentFrame + 1) % MAX_FRAME_IN_FLIGHT;
}

void Engine::initVulkan() {
	renderer.init("Blast Engine");
	createInstance();
	createSurface();
	getPhysicalDevice();
	createLogicalDevice();
	getQueueFamilies();
	createSwapChain();
	createImageViews();
	createGraphicPipeline();
	createCommandPool();
	createVertexBuffer();
	createIndexBuffer();
	createCommandBuffers();
	createSyncObjects();
}


void Engine::mainLoop() {
	while (engineRunning) {
		engineRunning = renderer.loop();
		drawFrame();
	}
	vkDevice.waitIdle();
}

void Engine::cleanUpSwapChain() {
	vkb::destroy_swapchain(vkbSwapChain);
	for(vk::ImageView& imageView : swapChainImageViews) {
		vkDevice.destroyImageView(imageView);
	}
}

void Engine::cleanUp() {
	cleanUpSwapChain();
	vkDevice.destroyPipelineLayout(pipelineLayout);
	vbo.clean();
	ibo.clean();
	vkDevice.destroyPipeline(graphicsPipeline);
	vkDevice.destroyCommandPool(commandPool);
	for(size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) { 
		vkDevice.destroySemaphore(renderFinishedSemaphores[i]);
		vkDevice.destroySemaphore(imageAvailableSemaphores[i]);
		vkDevice.destroyFence(inFlightFences[i]);
	}
	vkb::destroy_device(vkbDevice);
	vkInstance.destroySurfaceKHR(surface);
	vkb::destroy_instance(vkbInstance);
	renderer.clean();
	std::println("clean");
}

void Engine::run() {
	mainLoop();
	cleanUp();
	std::println("Test run.");
}