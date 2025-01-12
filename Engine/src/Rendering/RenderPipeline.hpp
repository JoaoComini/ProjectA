#pragma once

#include "RenderContext.hpp"

#include "Pass/ForwardPass.hpp"
#include "Pass/ShadowPass.hpp"
#include "Pass/CompositionPass.hpp"

namespace Engine
{
	class Scene;

	class RenderPipeline
	{
	public:
		RenderPipeline(RenderContext& renderContext, Scene& scene);

		void Draw(Vulkan::CommandBuffer& commandBuffer);
	private:
		void SetupGBufferPass();
		void SetupShadowPass();
		void SetupCompositionPass();

		std::unique_ptr<RenderTarget> CreateGBufferPassTarget();
		std::unique_ptr<RenderTarget> CreateShadowPassTarget();

		void DrawShadowPass(Vulkan::CommandBuffer& commandBuffer);
		void DrawMainPass(Vulkan::CommandBuffer& commandBuffer);
		void DrawCompositionPass(Vulkan::CommandBuffer& commandBuffer);

		void SetViewportAndScissor(Vulkan::CommandBuffer& commandBuffer, VkExtent2D extent);

		std::unique_ptr<ForwardPass> mainPass;
		std::unique_ptr<ShadowPass> shadowPass;
		std::unique_ptr<CompositionPass> compositionPass;

		std::unique_ptr<RenderTarget> shadowTarget;
		std::unique_ptr<RenderTarget> gBufferTarget;

		RenderContext& renderContext;
		Scene& scene;
	};
}