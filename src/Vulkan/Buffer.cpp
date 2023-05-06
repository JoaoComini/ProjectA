#include "Buffer.hpp"

namespace Vulkan
{
    Buffer::Buffer(const Device &device, void *data, uint32_t size) : device(device)
    {
        VkBufferCreateInfo createBufferInfo{};
        createBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createBufferInfo.size = size;
        createBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        createBufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo createAllocationInfo = {};
        createAllocationInfo.usage = VMA_MEMORY_USAGE_AUTO;
        createAllocationInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        vmaCreateBuffer(device.GetAllocator(), &createBufferInfo, &createAllocationInfo, &handle, &allocation, &allocationInfo);

        memcpy(allocationInfo.pMappedData, data, size);
    }

    Buffer::~Buffer()
    {
        vmaDestroyBuffer(device.GetAllocator(), handle, allocation);
    }

    VkBuffer Buffer::GetHandle() const
    {
        return handle;
    }

} // namespace Vulkan
