#include "Sampler.hpp"


namespace Vulkan
{

	Sampler::Sampler(const Device& device, VkSamplerCreateInfo info) : device(device)
	{
		auto properties = device.GetPhysicalDeviceProperties();

		if (vkCreateSampler(device.GetHandle(), &info, nullptr, &handle) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	Sampler::~Sampler()
	{
		vkDestroySampler(device.GetHandle(), handle, nullptr);
	}
}