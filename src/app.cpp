#include "app.hpp"
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
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Blast Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pEngineName = "Blast Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	if (checkExtension() == false) throw std::runtime_error("All the required extension are not available\n");
	uint32_t extensionCount;
	std::vector<const char*> extensions = getRequiredExtension();
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!\n");
	}
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create instance\n");
}

bool App::checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	bool hasLayers = false;
	for (size_t i = 0; i < validationLayers.size(); i++) {
		hasLayers = false;
		std::for_each(availableLayers.begin(), availableLayers.end(), [&](const VkLayerProperties& layerProp) {
			//become and keep true when strcmp is true one time
			hasLayers = hasLayers || strcmp(layerProp.layerName, validationLayers[i]);
			});
	}
	return hasLayers;
}

VKAPI_ATTR VkBool32 VKAPI_CALL App::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) 
{
	
	if(messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

void App::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
}

void App::setupDebugMessenger() {
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	populateDebugMessengerCreateInfo(createInfo);

	if(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		throw std::runtime_error("failed to set up debug messenger");
}

VkResult App::CreateDebugUtilsMessengerEXT(VkInstance instance,
									const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
									const VkAllocationCallbacks* pAllocator,
									VkDebugUtilsMessengerEXT* pDebugMessenger) 
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void App::DestroyDebugUtilsMessengerEXT(VkInstance instance, 
										VkDebugUtilsMessengerEXT debugMessenger,
										const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

std::vector<const char*> App::getRequiredExtension() {
	uint32_t windowExtensionCount = 0;
	const char** windowExtensions;
	windowExtensions = renderer.getExtensions(windowExtensionCount);
	std::vector<const char*> extensions(windowExtensions, windowExtensions + windowExtensionCount);
	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}

bool App::checkExtension() {
	uint32_t extensionCount;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	const char** extensionsToCheck = renderer.getExtensions(extensionCount);
	
	
	bool hasExtensions = false;
	for (size_t i = 0; i < extensionCount; i++) {
		hasExtensions = false;
		std::for_each(extensions.begin(), extensions.end(), [&](const VkExtensionProperties& ext) {
			hasExtensions = hasExtensions || strcmp(ext.extensionName, extensionsToCheck[i]);
			});

		if(!hasExtensions)
			break;
	}
	return hasExtensions;
}

bool App::checkDeviceExtension(const VkPhysicalDevice& device) {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
	
	bool hasExtensions = false;
	for (size_t i = 0; i < deviceExtensions.size(); i++) {
		hasExtensions = false;
		std::for_each(extensions.begin(), extensions.end(), [&](const VkExtensionProperties& ext) {
			hasExtensions = hasExtensions || strcmp(ext.extensionName, deviceExtensions[i]);
			});
		
			if (!hasExtensions)
				break;
	}
	return hasExtensions;
}

VkSurfaceFormatKHR App::chooseSwapSurfaceFormats(const std::vector<VkSurfaceFormatKHR>& surfaceFormats) {
	VkSurfaceFormatKHR retFormat = surfaceFormats[0];
	std::for_each(surfaceFormats.begin(), surfaceFormats.end(),
				[&retFormat](const VkSurfaceFormatKHR& format) {
					if(format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
						&& format.format == VK_FORMAT_R8G8B8A8_SRGB) {
							retFormat = format;
							return;
						}
				}	
	);
	return retFormat;
}

VkPresentModeKHR App::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes) {
	VkPresentModeKHR retMode = VK_PRESENT_MODE_FIFO_KHR;
	std::for_each(presentModes.begin(), presentModes.end(),
				[&retMode](const VkPresentModeKHR& mode) {
					if(mode == VK_PRESENT_MODE_MAILBOX_KHR) {
							retMode = mode;
							return;
					}
				}	
	);
	return retMode;
}

VkExtent2D App::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        renderer.getFrameBufferSize(width, height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

int App::ratePhysicalDevice(const VkPhysicalDevice& device) {
	int score = 0;
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;
	if (!deviceFeatures.multiDrawIndirect)
		return 0;
	
	score += deviceProperties.limits.maxImageDimension2D;
	QueueFamilyIndices indices = findQueueFamilies(device);
	if(!indices.graphicsFamily.has_value())
		score -= 1000;

	if(!checkDeviceExtension(device)) 
		score -= 1000;

	SwapChainSupportDetails support = querySwapChainSupport(device);
	if(!support.is_supported())
		score -= 1000;

	return score;
}

App::QueueFamilyIndices App::findQueueFamilies(const VkPhysicalDevice& device) {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyProperties.data());

	int i = 0;
	for(const VkQueueFamilyProperties& queueFamilyPropertie : queueFamilyProperties) {
		if(queueFamilyPropertie.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		VkBool32 surfaceIsSupported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &surfaceIsSupported);
		if(surfaceIsSupported) {
			indices.presentFamily = i;
		}

		if(indices.is_complete())
			break;
		i++;
	}
	
	return indices;
}

void App::pickPhysicalDevice() {
	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	if(physicalDeviceCount == 0)
		throw std::runtime_error("no physical device detected\n");
	
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
	std::multimap<int, VkPhysicalDevice> score_device;
	for(const VkPhysicalDevice& device : physicalDevices) {
		score_device.insert({ratePhysicalDevice(device), device});
	}

	auto suitableDevice = score_device.rbegin();

	if(suitableDevice->first <= 0)
		throw std::runtime_error("no suitable device detected\n");

	physicalDevice = suitableDevice->second;
}

App::SwapChainSupportDetails App::querySwapChainSupport(const VkPhysicalDevice& device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	
	uint32_t formatsCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatsCount, nullptr);
	if (formatsCount != 0)
	{
		details.formats.resize(formatsCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatsCount,details.formats.data());
	}

	uint32_t presentModesCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, nullptr);
	if (presentModesCount != 0)
	{
		details.presentModes.resize(presentModesCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, details.presentModes.data());
	}
	
	return details;
}

void App::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamily = {indices.graphicsFamily.value(), indices.presentFamily.value()};
	float queuePriority = 1;

	for(const uint32_t queue : uniqueQueueFamily) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queue;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfo.queueCount = 1;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.multiDrawIndirect = true;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) 
		throw std::runtime_error("failed to create logical device.\n");

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void App::createSurface() {
	renderer.createSurface(instance, &surface);
}

void App::createSwapChain() {
	SwapChainSupportDetails details = querySwapChainSupport(physicalDevice);

	VkExtent2D extent = chooseSwapExtent(details.capabilities);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(details.presentModes);
	VkSurfaceFormatKHR format = chooseSwapSurfaceFormats(details.formats);

	uint32_t imageCount = details.capabilities.minImageCount;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) 
    	imageCount = details.capabilities.maxImageCount;
	
	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageExtent = extent;
	createInfo.imageFormat = format.format;
	createInfo.imageColorSpace = format.colorSpace;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	std::array<uint32_t, 2> queueFamilyIndices = {indices.graphicsFamily.value(), indices.presentFamily.value()};
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	} else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	
	createInfo.preTransform = details.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;
	if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		throw std::runtime_error("failed to create swap chain!");
	
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
	swapChainFormat = createInfo.imageFormat;
	swapChainExtent = createInfo.imageExtent;
}

void App::recreateSwapChain() {
	vkDeviceWaitIdle(device);

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
	swapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainFormat;

		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
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

	file.seekg(0);
	file.read(buffer.data(), size_file);
	file.close();

	return buffer;
}

VkShaderModule App::createShaderModule(const std::vector<char> binaryShader) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = binaryShader.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(binaryShader.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void App::createGraphicPipeline() {
	std::vector<char> shaderVert = readFile("shaders/firstShaderVert.spv");
	std::vector<char> shaderFrag = readFile("shaders/firstShaderFrag.spv");

	VkShaderModule shaderVertModule = createShaderModule(shaderVert);
	VkShaderModule shaderFragModule = createShaderModule(shaderFrag);

	VkPipelineShaderStageCreateInfo shaderVertCreateInfo{};
	shaderVertCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderVertCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderVertCreateInfo.module = shaderVertModule;
	shaderVertCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderFragCreateInfo{};
	shaderFragCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderFragCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderFragCreateInfo.module = shaderFragModule;
	shaderFragCreateInfo.pName = "main";

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
	if(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
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

	if(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicPipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!!");
	}

	vkDestroyShaderModule(device, shaderVertModule, nullptr);
  vkDestroyShaderModule(device, shaderFragModule, nullptr);
}

void App::createRenderPass() {
	VkAttachmentDescription colorAttchement {};
	colorAttchement.format = swapChainFormat;
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

	if(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
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
		framebufferCreateInfo.height = swapChainExtent.height;
		framebufferCreateInfo.width = swapChainExtent.width;
		framebufferCreateInfo.layers = 1;

		if(vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &swapChainFrameBuffers[i])
			!= VK_SUCCESS
		) {
			throw std::runtime_error("Failed to create framebuffer");
		}
	}

}

void App::createCommandPool() {
	QueueFamilyIndices QueueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo commandPoolCreateInfo{};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = QueueFamilyIndices.graphicsFamily.value();

	if(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
		throw std::exception("Failed to create command pool!");
}

void App::createCommandBuffers() {
	VkCommandBufferAllocateInfo allocateInfo{};
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = commandBuffers.size();

	if(vkAllocateCommandBuffers(device, &allocateInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::exception("failed to allocate command buffers!");
	}
	
}

void App::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // optional
	beginInfo.pInheritanceInfo = nullptr; // optional

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::exception("Failed to begin the record of command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFrameBuffers[imageIndex];
	renderPassInfo.renderArea.offset = {0, 0};
	renderPassInfo.renderArea.extent = swapChainExtent;
	
	VkClearValue clearColor = {{{0.01, 0.01, 0.01, 1.0}}};
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.height = swapChainExtent.height;
	viewport.width = swapChainExtent.width;
	viewport.minDepth = 0;
	viewport.maxDepth = 1;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::exception("Failed to end command buffer.");
	}
}

void App::createSyncObjects() {
	VkSemaphoreCreateInfo semaphoreInfo {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for(size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
    vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
    vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create semaphores");
		}
	}

}

void App::drawFrame() {
	// Setup fence
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	
	// get image of swapchain and check if the swap chain is still OK
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire image of the swapchain.");
	}
	vkResetFences(device, 1, &inFlightFences[currentFrame]);
	
	
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
	std::array<VkSwapchainKHR, 1> swapChains = {swapChain};
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
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
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
	vkDeviceWaitIdle(device);
}

void App::cleanUpSwapChain() {
	
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	for (VkFramebuffer framebuffer : swapChainFrameBuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	for(VkImageView& imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}
}

void App::cleanUp() {
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	cleanUpSwapChain();
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyCommandPool(device, commandPool, nullptr);
	for(size_t i = 0; i < MAX_FRAME_IN_FLIGHT; i++) { 
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}
	vkDestroyDevice(device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
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
		cleanUp();
		return;
	}
	std::println("test\n");
	mainLoop();
	cleanUp();
	std::println("Test run.");
}