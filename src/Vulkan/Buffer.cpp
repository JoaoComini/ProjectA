#include "Buffer.hpp"

#include "Device.hpp"

namespace Vulkan
{
    Buffer::Buffer(const Device &device, uint32_t size) : device(device), size(size)
    {
    }

    Buffer::~Buffer()
    {
        vmaDestroyBuffer(device.GetAllocator(), handle, allocation);
    }

    void Buffer::SetData(void* data, uint32_t size, uint32_t offset)
    {
        memcpy(mappedData + offset, data, size);
        Flush();
    }

    void Buffer::Flush()
    {
        vmaFlushAllocation(device.GetAllocator(), allocation, 0, size);
    }

    bool Buffer::IsHostVisible()
    {
        return propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }

    uint32_t Buffer::GetSize() const
    {
        return size;
    }

    BufferBuilder BufferBuilder::Size(uint32_t size)
    {
        this->size = size;
        return *this;
    }

    BufferBuilder BufferBuilder::Persistent()
    {
        this->allocationCreate |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
        return *this;
    }

    BufferBuilder BufferBuilder::AllowTransfer()
    {
        this->allocationCreate |= VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT;
        return *this;
    }

    BufferBuilder BufferBuilder::SequentialWrite()
    {
        this->allocationCreate |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        return *this;
    }

    BufferBuilder BufferBuilder::BufferUsage(BufferUsageFlags bufferUsage)
    {
        this->bufferUsage = bufferUsage;
        return *this;
    }

    std::unique_ptr<Buffer> BufferBuilder::Build(const Device &device)
    {
        VkBufferCreateInfo bufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = static_cast<uint32_t>(bufferUsage),
        };

        VmaAllocationCreateInfo allocationCreateInfo{
            .flags = allocationCreate,
            .usage = VMA_MEMORY_USAGE_AUTO,
        };

        std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>(device, size);

        VmaAllocationInfo allocationInfo{};
        vmaCreateBuffer(device.GetAllocator(), &bufferCreateInfo, &allocationCreateInfo, &buffer->handle, &buffer->allocation, &allocationInfo);

        vmaGetAllocationMemoryProperties(device.GetAllocator(), buffer->allocation, &buffer->propertyFlags);

        buffer->mappedData = static_cast<uint8_t*>(allocationInfo.pMappedData);

        return buffer;
    }

}
