#include "Buffer.hpp"

namespace Vulkan
{
    Buffer::Buffer(Device &device) : device(device)
    {
    }

    Buffer::~Buffer()
    {
        vmaDestroyBuffer(device.GetAllocator(), handle, allocation);
    }

    VkBuffer Buffer::GetHandle() const
    {
        return handle;
    }

    BufferBuilder BufferBuilder::Data(void *data)
    {
        this->data = data;
        return *this;
    }

    BufferBuilder BufferBuilder::Size(uint32_t size)
    {
        this->size = size;
        return *this;
    }

    BufferBuilder BufferBuilder::AllocationCreate(AllocationCreateFlags allocationCreate)
    {
        this->allocationCreate = allocationCreate;
        return *this;
    }

    BufferBuilder BufferBuilder::BufferUsage(BufferUsageFlags bufferUsage)
    {
        this->bufferUsage = bufferUsage;
        return *this;
    }

    std::unique_ptr<Buffer> BufferBuilder::Build(Device &device)
    {
        VkBufferCreateInfo bufferCreateInfo{
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = bufferUsage,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

        VmaAllocationCreateInfo allocationCreateInfo{
            .flags = allocationCreate,
            .usage = VMA_MEMORY_USAGE_AUTO,
        };

        std::unique_ptr<Buffer> buffer = std::make_unique<Buffer>(device);
        vmaCreateBuffer(device.GetAllocator(), &bufferCreateInfo, &allocationCreateInfo, &buffer->handle, &buffer->allocation, &buffer->allocationInfo);

        if (data == nullptr)
        {
            return buffer;
        }

        VkMemoryPropertyFlags propertyFlags;
        vmaGetAllocationMemoryProperties(device.GetAllocator(), buffer->allocation, &propertyFlags);

        if (propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT && allocationCreate >= AllocationCreateFlags::MAPPED)
        {
            memcpy(buffer->allocationInfo.pMappedData, data, size);
        }
        else
        {
            VkBuffer staging;

            VkBufferCreateInfo stagingCreateInfo{
                .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                .size = size,
                .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            };

            VmaAllocationCreateInfo stagingAllocationCreateInfo{
                .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                .usage = VMA_MEMORY_USAGE_AUTO,
            };

            VmaAllocation stagingAllocation;
            VmaAllocationInfo stagingAllocationInfo;
            vmaCreateBuffer(device.GetAllocator(), &stagingCreateInfo, &stagingAllocationCreateInfo, &staging, &stagingAllocation, &stagingAllocationInfo);

            memcpy(stagingAllocationInfo.pMappedData, data, size);

            device.CopyBuffer(staging, buffer->handle, size);

            vmaDestroyBuffer(device.GetAllocator(), staging, stagingAllocation);
        }

        return buffer;
    }

} // namespace Vulkan
