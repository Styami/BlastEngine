#include "descriptor.hpp"
#include "texture.hpp"
#include <vulkan/vulkan_structs.hpp>

be::Descriptor::Descriptor() :
    m_device(nullptr)
{}

be::Descriptor::Descriptor(vk::Device device) :
    m_device(device)
{}

be::Descriptor::Descriptor(const Descriptor& another) :
    m_device(another.m_device),
    m_descriptorSetLayout(another.m_descriptorSetLayout),
    m_descriptorSets(another.m_descriptorSets)
{}

be::Descriptor& be::Descriptor::operator=(const Descriptor& another) {
    m_device = another.m_device;
    m_descriptorSetLayout = another.m_descriptorSetLayout;
    m_descriptorSets = another.m_descriptorSets;

    return *this;
}

void be::Descriptor::createSetLayout(const std::vector<vk::DescriptorSetLayoutBinding>& descriptorSetLayoutBinding) {
    vk::DescriptorSetLayoutCreateInfo descSetLayoutInfo = vk::DescriptorSetLayoutCreateInfo(
		{},
		descriptorSetLayoutBinding.size(),
		descriptorSetLayoutBinding.data()
	);
    m_layoutSize = descriptorSetLayoutBinding.size();
    m_descriptorSetLayout = m_device.createDescriptorSetLayout(descSetLayoutInfo);
}

void be::Descriptor::createPool(const std::vector<vk::DescriptorPoolSize>& createInfo, int numFrame) {
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo = vk::DescriptorPoolCreateInfo(
		vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		numFrame,
        createInfo.size(),
		createInfo.data()
	);
	m_descriptorPool = m_device.createDescriptorPool(descriptorPoolCreateInfo);
}

void be::Descriptor::createSet(size_t numberFrame, const std::vector<be::Buffer>& buffers, const be::Buffer& ssbo, const std::vector<be::Texture>& textures) {
    std::vector layouts = std::vector<vk::DescriptorSetLayout>(numberFrame, m_descriptorSetLayout);
    vk::DescriptorSetAllocateInfo allocInfo = vk::DescriptorSetAllocateInfo(
		m_descriptorPool,
		numberFrame,
		layouts.data()
	);
    m_descriptorSets = m_device.allocateDescriptorSets(allocInfo);
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    std::vector<std::vector<vk::DescriptorImageInfo>> imagesInfo;
    imagesInfo.resize(numberFrame);
    std::vector<std::vector<vk::DescriptorBufferInfo>> buffersInfo;
    buffersInfo.resize(numberFrame);
    std::vector<std::vector<vk::DescriptorBufferInfo>> ssbosInfo;
    ssbosInfo.resize(numberFrame);
    for (size_t i = 0; i < numberFrame; i++) {
        vk::DescriptorBufferInfo uboInfo = vk::DescriptorBufferInfo(
            buffers[i].getBuffer(),
            0,
            buffers[i].getSize()
        );
        buffersInfo[i].push_back(uboInfo);
        vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet(
            m_descriptorSets[i],
            0,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            buffersInfo[i].data()
        );
        writeDescriptorSets.push_back(writeDescriptorSet);
        
        vk::DescriptorBufferInfo ssboInfo = vk::DescriptorBufferInfo(
            ssbo.getBuffer(),
            0,
            ssbo.getSize()
        );
        ssbosInfo[i].push_back(ssboInfo);
        writeDescriptorSet = vk::WriteDescriptorSet(
            m_descriptorSets[i],
            2,
            0,
            1,
            vk::DescriptorType::eStorageBuffer,
            {},
            ssbosInfo[i].data()
        );
        writeDescriptorSets.push_back(writeDescriptorSet);

        for (const be::Texture& texture : textures) {

            vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo(
            be::Texture::getSampler(),
            texture.getImageView(),
            vk::ImageLayout::eShaderReadOnlyOptimal
            );
            imagesInfo[i].push_back(imageInfo);
        }
        writeDescriptorSet = vk::WriteDescriptorSet(
            m_descriptorSets[i],
            1,
            0,
            imagesInfo[i].size(),
            vk::DescriptorType::eCombinedImageSampler,
            imagesInfo[i].data()
        );
        writeDescriptorSets.push_back(writeDescriptorSet);
    }
    m_device.updateDescriptorSets(writeDescriptorSets, {});
}

const vk::DescriptorSetLayout& be::Descriptor::getLayout() const {
    return m_descriptorSetLayout;
}

size_t be::Descriptor::getLayoutSize() const {
    return m_layoutSize;
}

const std::vector<vk::DescriptorSet>& be::Descriptor::getSets() const {
    return m_descriptorSets;
}

void be::Descriptor::clean() {
    m_device.destroyDescriptorSetLayout(m_descriptorSetLayout);
    m_device.destroyDescriptorPool(m_descriptorPool);
}