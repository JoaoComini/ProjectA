#pragma once

#include "RenderContext.hpp"

#include "Pass/ForwardPass.hpp"
#include "Pass/ShadowPass.hpp"
#include "Pass/CompositionPass.hpp"

#include "RenderGraph/RenderGraphAllocator.hpp"

#include "ShaderCache.hpp"

namespace Engine
{
	class Scene;

	struct BackbufferData
	{
		RenderGraphResourceHandle backbuffer;
	};

	class RenderPipeline
	{
	public:
		RenderPipeline(RenderContext& renderContext, Scene& scene);

		void Draw(Vulkan::CommandBuffer& commandBuffer);
	private:
		std::unique_ptr<ForwardPass> mainPass;
		std::unique_ptr<ShadowPass> shadowPass;
		std::unique_ptr<CompositionPass> compositionPass;

		std::unordered_map<RenderTextureSampler, std::unique_ptr<Vulkan::Sampler>> samplers;
		std::unique_ptr<RenderGraphAllocator> allocator;

		ShaderCache shaderCache;

		RenderContext& renderContext;
		Scene& scene;
	};
}