#include "buffer.hpp"
#include "utils.hpp"

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

be::Buffer::Buffer(Buffer&& another) :
    m_buffer(std::move(another.m_buffer)),
    m_device(std::move(another.m_device)),
    m_size(std::move(another.m_size)),
    m_memory(std::move(another.m_memory))
{
	another.m_buffer = VK_NULL_HANDLE;
	another.m_device = VK_NULL_HANDLE;
	another.m_size = 0;
	another.m_memory = VK_NULL_HANDLE;
}

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

be::Buffer& be::Buffer::operator=(Buffer&& another) {
    if (*this != another) {
		m_buffer = another.m_buffer;
		another.m_buffer = VK_NULL_HANDLE;
		m_device = another.m_device;
		another.m_device = VK_NULL_HANDLE;
		m_size = another.m_size;
		another.m_size = 0;
		m_memory = another.m_memory;
		another.m_memory = VK_NULL_HANDLE;
    }
    return *this;
}

bool be::Buffer::operator==(const Buffer& another) const {
	return m_buffer == another.m_buffer
			&& m_device == another.m_device
			&& m_size == another.m_size
			&& m_memory == another.m_memory;
}

bool be::Buffer::operator!=(const Buffer& another) const {
	return !(*this == another);
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