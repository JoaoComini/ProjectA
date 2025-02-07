#include "RenderPipeline.hpp"

#include "Scene/Scene.hpp"

#include "Vulkan/Swapchain.hpp"

#include <Shaders/embed.gen.hpp>

namespace Engine
{
    RenderPipeline::RenderPipeline(RenderContext& renderContext, Scene& scene)
		: renderContext(renderContext), scene(scene)
    {
		SetupShadowPass();
		SetupGBufferPass();
		SetupCompositionPass();
    }

	void RenderPipeline::SetupGBufferPass()
	{
		gBufferTarget = CreateGBufferPassTarget();

		auto forwardVert = embed::Shaders::get("forward.vert.glsl");
		auto forwardFrag = embed::Shaders::get("forward.frag.glsl");

		auto forwardSubpass = std::make_unique<ForwardPass>(
			renderContext,
			Vulkan::ShaderSource{ std::vector<uint8_t>{ forwardVert.begin(), forwardVert.end() }},
			Vulkan::ShaderSource{ std::vector<uint8_t>{ forwardFrag.begin(), forwardFrag.end() } },
			scene,
			shadowTarget.get()
		);

		//auto skyboxVert = embed::Shaders::get("skybox.vert.glsl");
		//auto skyboxFrag = embed::Shaders::get("skybox.frag.glsl");

		//auto skyboxSubpass = std::make_unique<SkyboxPass>(
		//	renderContext,
		//	Vulkan::ShaderSource{ std::vector<uint8_t>{ skyboxVert.begin(), skyboxVert.end() }},
		//	Vulkan::ShaderSource{ std::vector<uint8_t>{ skyboxFrag.begin(), skyboxFrag.end() } },
		//	scene
		//);

		mainPass = std::make_unique<ForwardPass>(
			renderContext,
			Vulkan::ShaderSource{ std::vector<uint8_t>{ forwardVert.begin(), forwardVert.end() } },
			Vulkan::ShaderSource{ std::vector<uint8_t>{ forwardFrag.begin(), forwardFrag.end() } },
			scene,
			shadowTarget.get()
		);
	}

	std::unique_ptr<RenderTarget> RenderPipeline::CreateGBufferPassTarget()
	{
		auto extent = renderContext.GetSwapchain().GetImageExtent();

		auto colorResolveAttachment = RenderAttachment::Builder(renderContext.GetDevice())
			.Usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
			.Format(VK_FORMAT_R16G16B16A16_SFLOAT)
			.Extent(extent)
			.Build();

		auto colorAttachment = RenderAttachment::Builder(renderContext.GetDevice())
			.Format(VK_FORMAT_R16G16B16A16_SFLOAT)
			.Extent(extent)
			.SampleCount(renderContext.GetDevice().GetMaxSampleCount())
			.ClearValue({ .color = {0.f, 0.f , 0.f, 1.f} })
			.LoadStoreInfo({
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			})
			.Resolve(std::move(colorResolveAttachment))
			.Build();

		auto depthAttachment = RenderAttachment::Builder(renderContext.GetDevice())
			.Usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.Format(renderContext.GetPhysicalDevice().GetSupportedDepthFormat())
			.SampleCount(renderContext.GetDevice().GetMaxSampleCount())
			.Extent(extent)
			.ClearValue({ .depthStencil = {0.f, 0} })
			.LoadStoreInfo({
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			})
			.Build();

		auto target = std::make_unique<RenderTarget>();
		target->AddColorAttachment(std::move(colorAttachment));
		target->SetDepthAttachment(std::move(depthAttachment));

		return std::move(target);
	}

	void RenderPipeline::SetupShadowPass()
	{
		shadowTarget = CreateShadowPassTarget();

		auto shadowmapVert = embed::Shaders::get("shadowmap.vert.glsl");
		auto shadowmapFrag = embed::Shaders::get("shadowmap.frag.glsl");

		auto vertexSource = Vulkan::ShaderSource{ std::vector<uint8_t>{ shadowmapVert.begin(), shadowmapVert.end() }};
		auto fragmentSource = Vulkan::ShaderSource{ std::vector<uint8_t>{ shadowmapFrag.begin(), shadowmapFrag.end() }};

		shadowPass = std::make_unique<ShadowPass>(renderContext,
			std::move(vertexSource),
			std::move(fragmentSource),
			scene
		);
	}

	std::unique_ptr<RenderTarget> RenderPipeline::CreateShadowPassTarget()
	{
		auto attachment = RenderAttachment::Builder(renderContext.GetDevice())
			.Usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
			.Extent({ 2048, 2048 })
			.ClearValue({ .depthStencil { 0.f, 0} })
			.Format(renderContext.GetPhysicalDevice().GetSupportedDepthFormat(true))
			.Build();

		auto target = std::make_unique<RenderTarget>();

		target->SetDepthAttachment(std::move(attachment));

		return std::move(target);
	}

	void RenderPipeline::SetupCompositionPass()
	{
		auto compositionVert = embed::Shaders::get("composition.vert.glsl");
		auto compositionFrag = embed::Shaders::get("composition.frag.glsl");

		auto vertexSource = Vulkan::ShaderSource{ std::vector<uint8_t>{ compositionVert.begin(), compositionVert.end() } };
		auto fragmentSource = Vulkan::ShaderSource{ std::vector<uint8_t>{ compositionFrag.begin(), compositionFrag.end() } };

		compositionPass = std::make_unique<CompositionPass>(
			renderContext,
			std::move(vertexSource),
			std::move(fragmentSource),
			gBufferTarget.get()
		);
	}

	void RenderPipeline::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		DrawShadowPass(commandBuffer);
		DrawMainPass(commandBuffer);
		DrawCompositionPass(commandBuffer);
	}

	void RenderPipeline::DrawShadowPass(Vulkan::CommandBuffer& commandBuffer)
	{
		VkExtent2D extent = shadowTarget->GetExtent();

		SetViewportAndScissor(commandBuffer, extent);

		auto& depthAttachment = shadowTarget->GetDepthAttachment();
		
		{
			Vulkan::ImageMemoryBarrierInfo barrier{};
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

			commandBuffer.ImageMemoryBarrier(depthAttachment.GetView(), barrier);
		}

		shadowPass->Execute(commandBuffer, *shadowTarget);

		commandBuffer.EndRendering();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};
			barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			commandBuffer.ImageMemoryBarrier(depthAttachment.GetView(), barrier);
		}
	}

	void RenderPipeline::DrawMainPass(Vulkan::CommandBuffer& commandBuffer)
	{
		{
			if (
				gBufferTarget->GetExtent().width != renderContext.GetCurrentFrame().GetTarget().GetExtent().width ||
				gBufferTarget->GetExtent().height != renderContext.GetCurrentFrame().GetTarget().GetExtent().height
				)
			{
				*gBufferTarget = std::move(*CreateGBufferPassTarget());
			}
		}

		VkExtent2D extent = gBufferTarget->GetExtent();

		SetViewportAndScissor(commandBuffer, extent);

		auto& colorAttachment = gBufferTarget->GetColorAttachment(0);

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};

			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			if (auto& resolve = colorAttachment.GetResolve())
			{
				commandBuffer.ImageMemoryBarrier(resolve->GetView(), barrier);
			}
			else
			{
				commandBuffer.ImageMemoryBarrier(colorAttachment.GetView(), barrier);
			}
		}

		mainPass->Execute(commandBuffer, *gBufferTarget);

		commandBuffer.EndRendering();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};
			barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			if (auto& resolve = colorAttachment.GetResolve())
			{
				commandBuffer.ImageMemoryBarrier(resolve->GetView(), barrier);
			}
			else
			{
				commandBuffer.ImageMemoryBarrier(colorAttachment.GetView(), barrier);
			}
		}
	}

	void RenderPipeline::DrawCompositionPass(Vulkan::CommandBuffer& commandBuffer)
	{
		auto& target = renderContext.GetCurrentFrame().GetTarget();

		VkExtent2D extent = target.GetExtent();

		SetViewportAndScissor(commandBuffer, extent);

		auto& attachments = target.GetColorAttachment(0);

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};

			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			commandBuffer.ImageMemoryBarrier(attachments.GetView(), barrier);
		}

		compositionPass->Execute(commandBuffer, target);
	}

	void RenderPipeline::SetViewportAndScissor(Vulkan::CommandBuffer& commandBuffer, VkExtent2D extent)
	{
		commandBuffer.SetViewport(
			{{
				.width = static_cast<float>(extent.width),
				.height = static_cast<float>(extent.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f,
			}}
		);

		commandBuffer.SetScissor(
			{{
				.extent = extent
			}}
		);
	}
}