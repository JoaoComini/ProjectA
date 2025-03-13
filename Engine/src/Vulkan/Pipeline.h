#pragma once

#include "Device.h"
#include "PipelineLayout.h"
#include "Resource.h"

#include "PipelineState.h"

namespace Vulkan
{
	class Pipeline : public Resource<VkPipeline>
	{
	public:
		Pipeline(const Device& device, const PipelineState& state);
		~Pipeline();

	private:
		const Device& device;
	};
};
