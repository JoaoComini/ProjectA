#pragma once

#include "Pass.hpp"

namespace Engine
{
	class CompositionPass : public Pass
	{
	public:
		CompositionPass(
			RenderContext& renderContext,
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