#include "app.hpp"
#include <stdexcept>
#include <print>
#include <iostream>
#include <algorithm>


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

	if (checkExtension() == false) throw std::runtime_error("All the required extension are not available");
	uint32_t extensionCount;
	const char** extensions = renderer.getExtensions(extensionCount);
	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensions;

	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS) throw std::runtime_error("failed to create instance");
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

void App::initVulkan() {
	renderer.init("Blast Engine");
	createInstance();
}


void App::mainLoop() {
	renderer.loop();
}

void App::cleanUp() {
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
	}
	mainLoop();
	cleanUp();
	std::println("Test run.");
}