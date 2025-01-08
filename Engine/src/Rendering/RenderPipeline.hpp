#pragma once

#include "Pass/Pass.hpp"

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

		std::unique_ptr<Pass> mainPass;
		std::unique_ptr<Pass> shadowPass;
		std::unique_ptr<Pass> compositionPass;

		std::unique_ptr<RenderTarget> shadowTarget;
		std::unique_ptr<RenderTarget> gBufferTarget;

		RenderContext& renderContext;
		Scene& scene;
	};
}