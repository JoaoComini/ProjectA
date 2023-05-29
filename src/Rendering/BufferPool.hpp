#pragma once

#include "Vulkan/Buffer.hpp"
#include "Vulkan/Device.hpp"

namespace Rendering
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

	class BufferPool
	{
	public:
		BufferPool(const Vulkan::Device& device);
		~BufferPool() = default;

		BufferAllocation Allocate(uint32_t size);
		void Reset();

	private:
		std::unique_ptr<Vulkan::Buffer> buffer;

		uint32_t alignment;
		uint32_t offset = 0;
	};
}