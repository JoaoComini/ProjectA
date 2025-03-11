#pragma once

#include "Device.hpp"
#include "PipelineLayout.hpp"
#include "Resource.hpp"

#include "PipelineState.hpp"

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
