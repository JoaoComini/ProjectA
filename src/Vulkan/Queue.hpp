#pragma once

#include <vulkan/vulkan.h>

#include "Resource.hpp"
#include "CommandBuffer.hpp"

namespace Vulkan
{
	class Device;


	class Queue : public Resource<VkQueue>
	{
	public:
		enum Type { Present, Graphics };

		Queue(const Device& device, uint32_t familyIndex);
		~Queue() = default;

		void Submit(const CommandBuffer& commandBuffer);
		void WaitIdle();

		uint32_t GetFamilyIndex() const;

	private:
		const Device& device;
		uint32_t familyIndex;

	};
}
