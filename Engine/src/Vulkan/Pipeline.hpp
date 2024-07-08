#pragma once

#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "PipelineLayout.hpp"
#include "Resource.hpp"

#include "PipelineState.hpp"

namespace Vulkan
{
	class Pipeline : public Resource<VkPipeline>
	{
	public:
		Pipeline(const Device& device, PipelineState state);
		~Pipeline();

	private:
		const Device& device;
	};
};
