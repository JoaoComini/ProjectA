#pragma once

#include <vulkan/vulkan.h>

#include "Resource.hpp"
#include "Device.hpp"

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