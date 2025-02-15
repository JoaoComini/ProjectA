#pragma once

#include <vk_mem_alloc.h>

#include "Resource.hpp"

namespace Vulkan
{
	class Device;

	enum class BufferUsageFlags : uint32_t
	{
		Vertex = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		Index = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		Uniform = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		Staging = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	};

	class Buffer : public Resource<VkBuffer>
	{
	public:
		Buffer(const Device& device, uint32_t size);
		~Buffer();

		void SetData(void* data, uint32_t size, uint32_t offset = 0);
		void Flush();
		bool IsHostVisible();

		uint32_t GetSize() const;

	private:
		VmaAllocation allocation;
		VkMemoryPropertyFlags propertyFlags;

		const Device& device;

		uint8_t* mappedData;
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

		std::unique_ptr<Buffer> Build(const Device& device);

	private:
		uint32_t size = 0;
		BufferUsageFlags bufferUsage = BufferUsageFlags::Vertex;
		VmaAllocationCreateFlags allocationCreate = 0;
	};

} // namespace Vulkan
