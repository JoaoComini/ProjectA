#pragma once

#include "Vulkan/CommandBuffer.hpp"

#include "Pass/ForwardPass.hpp"
#include "Pass/ShadowPass.hpp"
#include "Pass/CompositionPass.hpp"

#include "RenderGraphAllocator.hpp"

#include "RenderCamera.hpp"
#include "RenderContext.hpp"
#include "ShaderCache.hpp"

namespace Engine
{
	class Window;
	class Scene;

	struct ResolutionSettings
	{
		int width;
		int height;
	};

	struct RendererSettings
	{
		ShadowSettings shadow;
		ResolutionSettings resolution{};
	};

	struct BackbufferData
	{
		RenderGraphResourceHandle<RenderTexture> target;
	};

	class Renderer
	{
	public:
		explicit Renderer(RenderContext& renderContext);
		~Renderer();

		void Draw(Vulkan::CommandBuffer& commandBuffer, Scene& scene, RenderCamera& camera, RenderAttachment& target);
	private:
		std::unordered_map<RenderTextureSampler, std::unique_ptr<Vulkan::Sampler>> samplers;
		std::unique_ptr<RenderGraphAllocator> allocator;

		RenderContext& renderContext;
		ShaderCache shaderCache;

		RendererSettings settings;
	};
}
