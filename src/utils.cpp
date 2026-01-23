#include "utils.hpp"
#include <stdexcept>

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

vk::Format findSupportedFormat(vk::PhysicalDevice physicalDevice, const std::vector<vk::Format>& formats, vk::ImageTiling tiling, vk::FormatFeatureFlags features) {
	for(const vk::Format& format : formats) {
		vk::FormatProperties prop = physicalDevice.getFormatProperties(format);
		if(tiling == vk::ImageTiling::eLinear && (prop.linearTilingFeatures & features) == features)
			return format;
		if(tiling == vk::ImageTiling::eOptimal && (prop.optimalTilingFeatures & features) == features)
			return format;
	}
	throw std::runtime_error("There is no supported Format!");
}

std::tuple<vk::DeviceMemory, vk::Image> createImage(
				vk::Device device,
				vk::PhysicalDevice physicalDevice,
				vk::ImageType type,
				vk::Format format,
				vk::Extent3D extent,
				uint32_t mipLevel,
				uint32_t arrayLayers,
				vk::SampleCountFlagBits sampleCount,
				vk::ImageTiling tiling,
				vk::ImageUsageFlags usage,
				vk::SharingMode sharingMode,
				vk::MemoryPropertyFlags properties
    		)
	{

    vk::ImageCreateInfo imageInfo = vk::ImageCreateInfo(
        {},
        type,
        format,
        extent,
        mipLevel,
        arrayLayers,
        sampleCount,
        tiling,
        usage,
        sharingMode
    );
    vk::Image image = device.createImage(imageInfo);
    vk::MemoryRequirements memoryRequirement = device.getImageMemoryRequirements(image);
    
    vk::MemoryAllocateInfo memoryInfo = vk::MemoryAllocateInfo(
        memoryRequirement.size,
        findMemoryType(memoryRequirement.memoryTypeBits, properties, physicalDevice)
    );
    vk::DeviceMemory memory = device.allocateMemory(memoryInfo);
    device.bindImageMemory(image, memory, 0);
	return {memory, image};
}

vk::ImageView createImageView(vk::Device device, vk::Image image, vk::ImageViewType type, vk::Format format, vk::ImageSubresourceRange imageSubresourceRange) {
    vk::ImageViewCreateInfo imageViewInfo = vk::ImageViewCreateInfo(
        {},
        image,
        type,
        format,
        {},
        imageSubresourceRange
    );
    
    return device.createImageView(imageViewInfo);
}