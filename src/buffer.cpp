#include "buffer.hpp"

be::Buffer::Buffer() :
    m_buffer(nullptr),
    m_device(nullptr),
    m_size(0),
    m_memory(nullptr)
{}

be::Buffer::Buffer(vk::Device device, vk::DeviceSize size) :
    m_buffer(nullptr),
    m_device(device),
    m_size(size),
    m_memory(nullptr)
{}

be::Buffer::Buffer(const Buffer& another) :
    m_buffer(another.m_buffer),
    m_device(another.m_device),
    m_size(another.m_size),
    m_memory(another.m_memory)
{}

be::Buffer& be::Buffer::operator=(const Buffer& another) {
    if (m_buffer != nullptr) {
        m_device.destroyBuffer(m_buffer);
        m_device.freeMemory(m_memory);    
    }

    m_buffer = another.m_buffer;
    m_device = another.m_device;
    m_size = another.m_size;
    m_memory = another.m_memory;

    return *this;
}

uint32_t be::Buffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties, vk::PhysicalDevice physicalDevice) {
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

void be::Buffer::create(vk::BufferUsageFlags usage, vk::SharingMode sharingMode, vk::PhysicalDevice physicalDevice) {
    vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo(
		{},
		m_size,
		usage,
		sharingMode
	);
	m_buffer = m_device.createBuffer(bufferInfo);

	vk::MemoryRequirements memoryRequirements = m_device.getBufferMemoryRequirements(m_buffer);
	vk::MemoryAllocateInfo memoryAllocateInfo = vk::MemoryAllocateInfo(
		memoryRequirements.size,
		findMemoryType(memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, physicalDevice)
	);
	
	m_memory = m_device.allocateMemory(memoryAllocateInfo);
    m_device.bindBufferMemory(m_buffer, m_memory, 0);
}

template<typename T>
void be::Buffer::map(const std::vector<T>&) {
    
}

template<>
void be::Buffer::map(const std::vector<Vertex>& vertices) {
    void* data = m_device.mapMemory(m_memory, 0, m_size);
    memcpy(data, vertices.data(), m_size);
    m_device.unmapMemory(m_memory);
}

template<>
void be::Buffer::map(const std::vector<int>& indices) {
    void* data = m_device.mapMemory(m_memory, 0, m_size);
    memcpy(data, indices.data(), m_size);
    m_device.unmapMemory(m_memory);
}


void be::Buffer::copyBuffer(be::Buffer& stagingBuffer, vk::CommandPool commandPool, vk::Queue graphicsQueue) {
	vk::CommandBufferAllocateInfo commandBufferInfo = vk::CommandBufferAllocateInfo(
		commandPool,
		vk::CommandBufferLevel::ePrimary,
		1
	);
	vk::CommandBuffer transferCommandBuffer = m_device.allocateCommandBuffers(commandBufferInfo).front();

	vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo(
		vk::CommandBufferUsageFlagBits::eOneTimeSubmit
	);
	transferCommandBuffer.begin(beginInfo);
	transferCommandBuffer.copyBuffer(stagingBuffer.m_buffer, m_buffer, vk::BufferCopy(0, 0, m_size));
	transferCommandBuffer.end();
	graphicsQueue.submit(vk::SubmitInfo(
		{},
		{},
		{},
		1,
		&transferCommandBuffer
	));
	graphicsQueue.waitIdle();

	m_device.freeCommandBuffers(commandPool, transferCommandBuffer);
    stagingBuffer.clean();
}

const vk::Buffer& be::Buffer::getBuffer() {
    return m_buffer;
}

void be::Buffer::clean(){
    m_device.destroyBuffer(m_buffer);
    m_device.freeMemory(m_memory);
}