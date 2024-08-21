#pragma once

#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"

namespace Engine
{
	class BufferAllocation
	{
	public:
		BufferAllocation(Vulkan::Buffer& buffer, uint32_t size, uint32_t offset);

		void SetData(void* data);

		const Vulkan::Buffer& GetBuffer() const;
		uint32_t GetSize() const;
		uint32_t GetOffset() const;

	private:
		Vulkan::Buffer& buffer;

		uint32_t size = 0;
		uint32_t offset = 0;
	};

	class BufferBlock
	{
	public:
		BufferBlock(const Vulkan::Device& device, uint32_t size, Vulkan::BufferUsageFlags usage);

		bool CanAllocate(uint32_t size);
		BufferAllocation Allocate(uint32_t size);
		void Reset();

	private:
		std::unique_ptr<Vulkan::Buffer> buffer;

		uint32_t alignment = 0;
		uint32_t offset = 0;
	};

	class BufferPool
	{
	public:
		BufferPool(const Vulkan::Device& device, Vulkan::BufferUsageFlags usage, uint32_t blockSize);
		~BufferPool() = default;

		BufferAllocation Allocate(uint32_t size);
		void Reset();

	private:
		const Vulkan::Device& device;
		Vulkan::BufferUsageFlags usage;
		uint32_t blockSize;

		std::vector<std::unique_ptr<BufferBlock>> blocks;
		uint32_t activeBlockIndex = 0;


	};
}