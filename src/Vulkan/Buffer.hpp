#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include "Device.hpp"

namespace Vulkan
{

    enum AllocationCreateFlags : uint32_t
    {
        MAPPED = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
        MAPPED_BAR = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT,
    };
 
    enum BufferUsageFlags : uint32_t
    {
        VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        INDEX = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        UNIFORM = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
    };

    class Buffer
    {
    public:
        Buffer(Device &device);
        ~Buffer();

        VkBuffer GetHandle() const;

    private:
        VkBuffer handle;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;

        Device &device;

        friend class BufferBuilder;
    };

    class BufferBuilder
    {
    public:
        BufferBuilder() = default;
        ~BufferBuilder() = default;

        BufferBuilder Data(void * data);
        BufferBuilder Size(uint32_t size);

        BufferBuilder AllocationCreate(AllocationCreateFlags allocationCreate);
        BufferBuilder BufferUsage(BufferUsageFlags bufferUsage);

        std::unique_ptr<Buffer> Build(Device &device);

    private:
        void *data = nullptr;
        uint32_t size;

        AllocationCreateFlags allocationCreate;
        BufferUsageFlags bufferUsage;
    };

} // namespace Vulkan
