#pragma once

#include "Subpass.hpp"

namespace Engine
{
	class CompositionSubpass : public Subpass
	{
	public:
		CompositionSubpass(
			Vulkan::Device& device,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			RenderTarget* gBufferTarget
		);

		void Draw(Vulkan::CommandBuffer& commandBuffer) override;

	private:
		RenderTarget* gBufferTarget;

		std::unique_ptr<Vulkan::Sampler> gBufferSampler;
	};

}