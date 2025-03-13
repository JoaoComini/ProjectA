#pragma once

#include "Vulkan/CommandBuffer.h"

#include "Pass/ForwardPass.h"
#include "Pass/ShadowPass.h"
#include "Pass/CompositionPass.h"

#include "RenderGraphAllocator.h"

#include "RenderCamera.h"
#include "RenderContext.h"
#include "ShaderCache.h"

namespace Engine
{
	class Window;
	class Scene;

	struct RendererSettings
	{
		ShadowSettings shadow;
	};

	struct BackBufferData
	{
		RenderGraphResourceHandle<RenderTexture> target;
	};

	struct FrameData
	{
		RenderGraphResourceHandle<RenderBuffer> camera;
	};

	struct LightData
	{
		RenderGraphResourceHandle<RenderBuffer> lights;
		RenderGraphResourceHandle<RenderBuffer> shadows;
	};


	class Renderer
	{
	public:
		explicit Renderer(RenderContext& renderContext);
		~Renderer();

		void Draw(Vulkan::CommandBuffer& commandBuffer, Scene& scene, RenderCamera& camera, RenderAttachment& target);
	private:
		void ImportBackBufferData(RenderGraph& graph, RenderGraphContext& context, RenderAttachment& target) const;
		void ImportFrameData(RenderGraph& graph, RenderGraphContext& context, RenderCamera& camera) const;
		void ImportLightsData(RenderGraph& graph, RenderGraphContext& context, Scene& scene) const;

		std::unordered_map<RenderTextureSampler, std::unique_ptr<Vulkan::Sampler>> samplers;
		std::unique_ptr<RenderGraphAllocator> allocator;

		RenderContext& renderContext;
		ShaderCache shaderCache;

		RendererSettings settings;
	};
}
