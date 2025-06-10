#include "app.hpp"
#include <stdexcept>
#include <print>
#include <iostream>
#include <algorithm>
#include <map>


App::App()
{
	std::println("Construct App.");
}

void App::createInstance() {
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Blast Engine";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
	appInfo.pEngineName = "Blast";
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
	
	if(messageSeverity >= VK_DEBUG_REPORT_WARNING_BIT_EXT)
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
	}
	return hasExtensions;
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

void App::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.multiDrawIndirect = true;

	VkDeviceCreateInfo deviceCreateInfo{};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
	deviceCreateInfo.queueCreateInfoCount = 1;
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

	deviceCreateInfo.enabledExtensionCount = 0;

	if(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device)) 
		throw std::runtime_error("failed to create logical device.\n");

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

void App::initVulkan() {
	renderer.init("Blast Engine");
	createInstance();
	setupDebugMessenger();
	pickPhysicalDevice();
}


void App::mainLoop() {
	renderer.loop();
}

void App::cleanUp() {
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	
	vkDestroyInstance(instance, nullptr);
	vkDestroyDevice(device, nullptr);
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
	}
	mainLoop();
	cleanUp();
	std::println("Test run.");
}