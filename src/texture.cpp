#include "texture.hpp"
#include <format>
#include "utils.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

be::Texture::Texture(const std::filesystem::path& name, vk::Queue queue) :
    m_queue(queue)
{
    loadImage(name);
}

void be::Texture::setDevice(vk::Device device) {
    be::Texture::m_device = device;
}

void be::Texture::setPhysicalDevice(vk::PhysicalDevice physicalDevice) {
    be::Texture::m_physicalDevice = physicalDevice;
}

void be::Texture::loadImage(const std::filesystem::path& name) {
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* pixels = stbi_load(name.c_str(), &m_width, &m_height, &m_texChannels, STBI_rgb_alpha);
    if (pixels == nullptr) {
        std::string errorMsg = std::format("Failed to load {} texture", name.c_str());
        throw std::runtime_error(errorMsg);
    }

    m_buffer = be::Buffer(m_device, m_height * m_width * 4);
    m_buffer.create(vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive, m_physicalDevice);
    m_buffer.map<stbi_uc>(std::vector<stbi_uc>(pixels, pixels + (m_width * m_height * 4)));
    stbi_image_free(pixels);
}

void be::Texture::createImage(vk::ImageType type,
                                vk::Format format,
                                uint32_t mipLevel,
                                uint32_t arrayLayers,
                                vk::SampleCountFlagBits sampleCount,
                                vk::ImageTiling tiling,
                                vk::ImageUsageFlags usage,
                                vk::SharingMode sharingMode,
                                vk::MemoryPropertyFlags properties
    ) {

    vk::ImageCreateInfo imageInfo = vk::ImageCreateInfo(
        {},
        type,
        format,
        vk::Extent3D(m_width, m_height, 1),
        mipLevel,
        arrayLayers,
        sampleCount,
        tiling,
        usage,
        sharingMode
    );
    m_image = m_device.createImage(imageInfo);
    vk::MemoryRequirements memory = m_device.getImageMemoryRequirements(m_image);
    
    vk::MemoryAllocateInfo memoryInfo = vk::MemoryAllocateInfo(
        memory.size,
        findMemoryType(memory.memoryTypeBits, properties, m_physicalDevice)
    );
    m_memory = m_device.allocateMemory(memoryInfo);
    m_device.bindImageMemory(m_image, m_memory, 0);
}

void be::Texture::copyBufferToImage(vk::CommandPool commandPool) {
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands(m_device, commandPool);
    vk::BufferImageCopy region = vk::BufferImageCopy(
        0,
        0,
        0,
        vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
        vk::Offset3D(0, 0, 0),
        vk::Extent3D(m_width, m_height, 1)
    );
    commandBuffer.copyBufferToImage(m_buffer.getBuffer(), m_image, vk::ImageLayout::eTransferDstOptimal,region);
    endSingleTimeCommands(m_device, commandPool, commandBuffer, m_queue);
}

void be::Texture::transitionImageLayout(vk::ImageLayout oldLayout, vk::ImageLayout newLayout, vk::CommandPool commandPool) {
    vk::ImageMemoryBarrier barrier;
    barrier.setOldLayout(oldLayout);
    barrier.setNewLayout(newLayout);

    vk::PipelineStageFlags srcStage, dstStage;
    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.setSrcAccessMask({});
        barrier.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);

        srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
        dstStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite);
        barrier.setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        srcStage = vk::PipelineStageFlagBits::eTransfer;
        dstStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument("Unsuported image Layout for transition");
    }
    vk::CommandBuffer commandBuffer = beginSingleTimeCommands(m_device, commandPool);
    barrier.setImage(m_image),
    barrier.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
    commandBuffer.pipelineBarrier(srcStage, dstStage, {}, {}, {}, barrier);
    endSingleTimeCommands(m_device, commandPool, commandBuffer, m_queue);
}

void be::Texture::createImageView(vk::Format format) {
    vk::ImageViewCreateInfo imageViewInfo = vk::ImageViewCreateInfo(
        {},
        m_image,
        vk::ImageViewType::e2D,
        format,
        {},
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1)
    );
    
    m_imageView = m_device.createImageView(imageViewInfo);
}

void be::Texture::createTextureSampler() {
    vk::PhysicalDeviceProperties properties = m_physicalDevice.getProperties();
    vk::SamplerCreateInfo samplerInfo = vk::SamplerCreateInfo(
        {},
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        {},
        vk::True,
        properties.limits.maxSamplerAnisotropy,
        vk::False,
        vk::CompareOp::eAlways
    );
    sampler = m_device.createSampler(samplerInfo);
}

vk::ImageView be::Texture::getImageView() const {
    return m_imageView;
}

vk::Image be::Texture::getImage() const {
    return m_image;
}

vk::Sampler be::Texture::getSampler() {
    return sampler;
}

vk::Buffer be::Texture::getBuffer() const {
    return m_buffer.getBuffer();
}

void be::Texture::clean() {
    m_buffer.clean();
    m_device.freeMemory(m_memory);
    m_device.destroyImage(m_image);
    m_device.destroyImageView(m_imageView);
}

void be::Texture::cleanSampler() {
    m_device.destroySampler(sampler);
}

