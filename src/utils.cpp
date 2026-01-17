#include "utils.hpp"

uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice) {
    vk::PhysicalDeviceMemoryProperties memProp;
	memProp = physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProp.memoryTypeCount; i++) {
		// This bitwise operation permits to know if the property flag can be OK for our properties
		// Then the equality permits to verify that the selected flag satisfy ALL our properties. 
		if ((typeFilter & (1 << i)) && (memProp.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("Failed to find a correct memory type!");
}

vk::CommandBuffer beginSingleTimeCommands(vk::Device device, vk::CommandPool commandPool) {
	vk::CommandBufferAllocateInfo allocateInfo = vk::CommandBufferAllocateInfo(
		commandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);
	vk::CommandBuffer commandBuffer = device.allocateCommandBuffers(allocateInfo).front();

	vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo(
		vk::CommandBufferUsageFlagBits::eOneTimeSubmit
	);
	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void endSingleTimeCommands(vk::Device device, vk::CommandPool commandPool, vk::CommandBuffer commandBuffer, vk::Queue graphicsQueue) {
	commandBuffer.end();

	vk::SubmitInfo submitInfo = vk::SubmitInfo(
		0,
		{},
		{},
		1,
		&commandBuffer
	);
	graphicsQueue.submit(submitInfo);
	graphicsQueue.waitIdle();
	device.freeCommandBuffers(commandPool, commandBuffer);
}