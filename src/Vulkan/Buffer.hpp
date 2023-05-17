#pragma once

#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

#include <memory>

#include "Resource.hpp"

namespace Vulkan
{
	class Device;

	enum class BufferUsageFlags : uint32_t
	{
		VERTEX = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		INDEX = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		UNIFORM = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		STAGING = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	};

	class Buffer : public Resource<VkBuffer>
	{
	public:
		Buffer(Device& device, uint32_t size);
		~Buffer();

		void SetData(void* data, uint32_t size);
		void Flush();
		bool IsHostVisible();

	private:
		VmaAllocation allocation;
		VmaAllocationInfo allocationInfo;
		VkMemoryPropertyFlags propertyFlags;

		Device& device;
		uint32_t size;

		friend class BufferBuilder;
	};

	class BufferBuilder
	{
	public:
		BufferBuilder() = default;
		~BufferBuilder() = default;

		BufferBuilder Size(uint32_t size);
		BufferBuilder Persistent();
		BufferBuilder AllowTransfer();
		BufferBuilder SequentialWrite();
		BufferBuilder BufferUsage(BufferUsageFlags bufferUsage);

		std::unique_ptr<Buffer> Build(Device& device);

	private:
		uint32_t size = 0;
		BufferUsageFlags bufferUsage = BufferUsageFlags::VERTEX;
		VmaAllocationCreateFlags allocationCreate = 0;
	};

} // namespace Vulkan
