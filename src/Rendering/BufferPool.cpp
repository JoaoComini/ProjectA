#include "BufferPool.hpp"

namespace Rendering
{
	BufferPool::BufferPool(const Vulkan::Device& device)
	{
		buffer = Vulkan::BufferBuilder()
			.Persistent()
			.SequentialWrite()
			.BufferUsage(Vulkan::BufferUsageFlags::UNIFORM)
			.Size(256 * 1024)
			.Build(device);

		auto properties = device.GetPhysicalDeviceProperties();

		alignment = properties.limits.minUniformBufferOffsetAlignment;
	}

	BufferAllocation BufferPool::Allocate(uint32_t size)
	{
		uint32_t aligned = (offset + alignment - 1) & ~(alignment - 1);

		offset = aligned + size;

		return BufferAllocation(*buffer, size, aligned);
	}

	void BufferPool::Reset()
	{
		offset = 0;
	}

	BufferAllocation::BufferAllocation(Vulkan::Buffer& buffer, uint32_t size, uint32_t offset) :
		buffer(buffer), size(size), offset(offset) { }

	const Vulkan::Buffer& BufferAllocation::GetBuffer() const
	{
		return buffer;
	}

	uint32_t BufferAllocation::GetSize() const
	{
		return size;
	}

	uint32_t BufferAllocation::GetOffset() const
	{
		return offset;
	}

	void BufferAllocation::SetData(void* data)
	{
		buffer.SetData(data, size, offset);
	}
}