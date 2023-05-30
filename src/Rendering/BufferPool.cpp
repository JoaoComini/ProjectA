#include "BufferPool.hpp"

namespace Rendering
{
	BufferPool::BufferPool(const Vulkan::Device& device, Vulkan::BufferUsageFlags usage)
		: device(device), usage(usage)
	{
		blocks.emplace_back(std::make_unique<BufferBlock>(device, blockSize, usage));
	}

	BufferAllocation BufferPool::Allocate(uint32_t size)
	{
		auto& activeBlock = blocks[activeBlockIndex];

		if (activeBlock->CanAllocate(size))
		{
			return activeBlock->Allocate(size);
		}

		if (activeBlockIndex + 1 < blocks.size())
		{
			activeBlockIndex++;
			return blocks[activeBlockIndex]->Allocate(size);
		}

		blocks.emplace_back(std::make_unique<BufferBlock>(device, blockSize, usage));
		
		activeBlockIndex++;
		return blocks[activeBlockIndex]->Allocate(size);
	}

	void BufferPool::Reset()
	{
		for (auto& block : blocks)
		{
			block->Reset();
		}

		activeBlockIndex = 0;
	}

	BufferBlock::BufferBlock(const Vulkan::Device& device, uint32_t size, Vulkan::BufferUsageFlags usage)
	{
		buffer = Vulkan::BufferBuilder()
			.Persistent()
			.SequentialWrite()
			.BufferUsage(usage)
			.Size(size)
			.Build(device);

		auto properties = device.GetPhysicalDeviceProperties();

		if (usage == Vulkan::BufferUsageFlags::UNIFORM)
		{
			alignment = properties.limits.minUniformBufferOffsetAlignment;
		}
	}

	bool BufferBlock::CanAllocate(uint32_t size)
	{
		uint32_t aligned = (offset + alignment - 1) & ~(alignment - 1);

		return buffer->GetSize() >= aligned + size;
	}

	BufferAllocation BufferBlock::Allocate(uint32_t size)
	{
		uint32_t aligned = (offset + alignment - 1) & ~(alignment - 1);

		offset = aligned + size;

		return BufferAllocation(*buffer, size, aligned);
	}

	void BufferBlock::Reset()
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