#include "descriptor.hpp"
#include "vertex.hpp"

be::Descriptor::Descriptor() :
    m_buffers(0, nullptr),
    m_device(nullptr),
    m_size(0),
    m_memories(0, nullptr),
    m_descriptorSetLayouts(0, nullptr),
    m_descriptorSets(0, nullptr),
    m_data(0, nullptr)
{}

be::Descriptor::Descriptor(vk::Device device, size_t numberFrame) :
    m_buffers(numberFrame, nullptr),
    m_device(device),
    m_size(0),
    m_memories(numberFrame, nullptr),
    m_descriptorSetLayouts(numberFrame, nullptr),
    m_descriptorSets(numberFrame, nullptr),
    m_data(numberFrame, nullptr)
{}

be::Descriptor::Descriptor(const Descriptor& another) :
    m_buffers(another.m_buffers),
    m_device(another.m_device),
    m_size(another.m_size),
    m_memories(another.m_memories),
    m_descriptorSetLayouts(another.m_descriptorSetLayouts),
    m_descriptorSets(another.m_descriptorSets),
    m_data(another.m_data)
{}

be::Descriptor& be::Descriptor::operator=(const Descriptor& another) {
    if (!m_buffers.empty()) {
        for (size_t i = 0; i < m_buffers.size(); i++) {
            m_device.destroyBuffer(m_buffers[i]);
            m_device.freeMemory(m_memories[i]);
        }
        m_data.clear();
    }

    m_buffers = another.m_buffers;
    m_device = another.m_device;
    m_size = another.m_size;
    m_memories = another.m_memories;
    m_descriptorSetLayouts = another.m_descriptorSetLayouts;
    m_descriptorSets = another.m_descriptorSets;
    m_data = another.m_data;

    return *this;
}

uint32_t be::Descriptor::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProp;
	memProp = m_physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProp.memoryTypeCount; i++) {
		// This bitwise operation permits to know if the property flag can be OK for our properties
		// Then the equality permits to verify that the selected flag satisfy ALL our properties. 
		if ((typeFilter & (1 << i)) && (memProp.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("Failed to find a correct memory type!");
}

be::Descriptor& be::Descriptor::setUsage(vk::BufferUsageFlags usage) {
    m_usage = usage;
    return *this;
}
be::Descriptor& be::Descriptor::setType(vk::DescriptorType type) {
    m_type = type;
    return *this;
}
be::Descriptor& be::Descriptor::setSharingMode(vk::SharingMode sharingMode) {
    m_sharingMode = sharingMode;
    return *this;
}
be::Descriptor& be::Descriptor::setShaderStage(vk::ShaderStageFlagBits shaderStage) {
    m_shaderStage = shaderStage;
    return *this;
}
be::Descriptor& be::Descriptor::setDeviceSize(vk::DeviceSize size) {
    m_size = size;
    return *this;
}
be::Descriptor& be::Descriptor::setPhysicalDevice(vk::PhysicalDevice physicalDevice) {
    m_physicalDevice = physicalDevice;
    return *this;
}

void be::Descriptor::build() {
    vk::DescriptorSetLayoutBinding uniformBinding = vk::DescriptorSetLayoutBinding(
		0,
		m_type,
		1,
        m_shaderStage
	);
	vk::DescriptorSetLayoutCreateInfo descriptorCreateInfo = vk::DescriptorSetLayoutCreateInfo(
		{},
		1,
		&uniformBinding
	);
    vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo(
		{},
		m_size,
		m_usage,
		m_sharingMode
	);

    for (size_t i = 0; i < m_buffers.size(); i++) {
        m_buffers[i] = m_device.createBuffer(bufferInfo);
        m_descriptorSetLayouts[i] = m_device.createDescriptorSetLayout(descriptorCreateInfo);
        
        vk::MemoryRequirements memoryRequirements = m_device.getBufferMemoryRequirements(m_buffers[i]);
        vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo(
            memoryRequirements.size,
            findMemoryType(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
        );
        
        m_memories[i] = m_device.allocateMemory(memoryAllocateInfo);
        m_device.bindBufferMemory(m_buffers[i], m_memories[i], 0);
    }
}

void be::Descriptor::map() {
    for (size_t i = 0; i < m_data.size(); i++) {
        m_data[i] = m_device.mapMemory(m_memories[i], 0, m_size);
    }
}

void be::Descriptor::allocate(const vk::DescriptorPool& descriptorPool) {
    vk::DescriptorSetAllocateInfo allocInfo = vk::DescriptorSetAllocateInfo(
		descriptorPool,
		m_descriptorSetLayouts.size(),
		m_descriptorSetLayouts.data()
	);
    m_descriptorSets = m_device.allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < m_buffers.size(); i++) {
        vk::DescriptorBufferInfo bufferInfo = vk::DescriptorBufferInfo(
            m_buffers[i],
            0,
            m_size
        );
        vk::WriteDescriptorSet writeDescriptorSet = vk::WriteDescriptorSet(
            m_descriptorSets[i],
            0,
            0,
            1,
            m_type,
            {},
            &bufferInfo
        );
        m_device.updateDescriptorSets(writeDescriptorSet, {});
    }
}

const std::vector<vk::Buffer>& be::Descriptor::getBuffers() const {
    return m_buffers;
}

const std::vector<vk::DescriptorSetLayout>& be::Descriptor::getLayouts() const {
    return m_descriptorSetLayouts;
}

const std::vector<vk::DescriptorSet>& be::Descriptor::getSets() const {
    return m_descriptorSets;
}

void be::Descriptor::clean(const vk::DescriptorPool& ) {
    for (size_t i = 0; i < m_buffers.size(); i++) {
        m_device.destroyBuffer(m_buffers[i]);
        m_device.freeMemory(m_memories[i]);
        m_device.destroyDescriptorSetLayout(m_descriptorSetLayouts[i]);
    }
}