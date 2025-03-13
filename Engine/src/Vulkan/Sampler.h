#pragma once

#include "Resource.h"
#include "Device.h"

namespace Vulkan
{
	class Sampler : public Resource<VkSampler>
	{
	public:
		Sampler(const Device &device, VkSamplerCreateInfo info);
		~Sampler();

	private:
		const Device& device;
	};
}