#pragma once

#include "Device.hpp"
#include "Resource.hpp"

namespace Vulkan
{
	class Fence : public Resource<VkFence>
	{
	public:
		Fence(const Device& device, bool signaled = true);
		~Fence();

		void Wait();
		void Reset();

	private:
		const Device& device;

	};
}