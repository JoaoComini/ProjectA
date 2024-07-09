#include "RenderPipeline.hpp"

#include "Scene/Scene.hpp"

#include "Subpass/ForwardSubpass.hpp"
#include "Subpass/SkyboxSubpass.hpp"
#include "Subpass/ShadowSubpass.hpp"
#include "Subpass/CompositionSubpass.hpp"

#include "Vulkan/Swapchain.hpp"

namespace Engine
{
    RenderPipeline::RenderPipeline(RenderContext& renderContext, Scene& scene)
		: renderContext(renderContext), scene(scene)
    {
		shadowCamera = std::make_unique<OrthographicCamera>(-200.f, 200.f, -200.f, 200.f, 120.f, -120.f);

		SetupShadowPass();
		SetupGBufferPass();
		SetupCompositionPass();
    }

	void RenderPipeline::SetupGBufferPass()
	{
		gBufferTarget = CreateGBufferPassTarget();

		auto forwardSubpass = std::make_unique<ForwardSubpass>(
			renderContext,
			Vulkan::ShaderSource{ "resources/shaders/forward.vert" },
			Vulkan::ShaderSource{ "resources/shaders/forward.frag" },
			scene,
			*shadowCamera,
			shadowTarget.get()
		);

		auto skyboxSubpass = std::make_unique<SkyboxSubpass>(
			renderContext,
			Vulkan::ShaderSource{ "resources/shaders/skybox.vert" },
			Vulkan::ShaderSource{ "resources/shaders/skybox.frag" },
			scene
		);

		std::vector<std::unique_ptr<Subpass>> subpasses;
		subpasses.push_back(std::move(forwardSubpass));
		subpasses.push_back(std::move(skyboxSubpass));

		mainPass = std::make_unique<Pass>(renderContext.GetDevice(), std::move(subpasses));
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

		auto vertexSource = Vulkan::ShaderSource{ "resources/shaders/shadowmap.vert" };
		auto fragmentSource = Vulkan::ShaderSource{ "resources/shaders/shadowmap.frag" };

		auto subpass = std::make_unique<ShadowSubpass>(
			renderContext,
			std::move(vertexSource),
			std::move(fragmentSource),
			scene,
			*shadowCamera
		);

		std::vector<std::unique_ptr<Subpass>> subpasses;
		subpasses.push_back(std::move(subpass));

		shadowPass = std::make_unique<Pass>(renderContext.GetDevice(), std::move(subpasses));
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
		auto vertexSource = Vulkan::ShaderSource{ "resources/shaders/composition.vert" };
		auto fragmentSource = Vulkan::ShaderSource{ "resources/shaders/composition.frag" };

		auto subpass = std::make_unique<CompositionSubpass>(
			renderContext,
			std::move(vertexSource),
			std::move(fragmentSource),
			gBufferTarget.get()
		);

		std::vector<std::unique_ptr<Subpass>> subpasses;
		subpasses.push_back(std::move(subpass));

		compositionPass = std::make_unique<Pass>(renderContext.GetDevice(), std::move(subpasses));
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

			commandBuffer.ImageMemoryBarrier(depthAttachment->GetView(), barrier);
		}

		shadowPass->Draw(commandBuffer, *shadowTarget);

		commandBuffer.EndRendering();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};
			barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			commandBuffer.ImageMemoryBarrier(depthAttachment->GetView(), barrier);
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

		auto& colorAttachments = gBufferTarget->GetColorAttachments();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};

			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			commandBuffer.ImageMemoryBarrier(colorAttachments[0]->GetView(), barrier);

			if (auto& resolve = colorAttachments[0]->GetResolve())
			{
				commandBuffer.ImageMemoryBarrier(resolve->GetView(), barrier);
			}
		}

		auto& depthAttachment = gBufferTarget->GetDepthAttachment();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

			commandBuffer.ImageMemoryBarrier(depthAttachment->GetView(), barrier);
		}

		mainPass->Draw(commandBuffer, *gBufferTarget);

		commandBuffer.EndRendering();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};
			barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			if (auto& resolve = colorAttachments[0]->GetResolve())
			{
				commandBuffer.ImageMemoryBarrier(resolve->GetView(), barrier);

				return;
			}

			commandBuffer.ImageMemoryBarrier(colorAttachments[0]->GetView(), barrier);
		}
	}

	void RenderPipeline::DrawCompositionPass(Vulkan::CommandBuffer& commandBuffer)
	{
		auto& target = renderContext.GetCurrentFrame().GetTarget();

		VkExtent2D extent = target.GetExtent();

		SetViewportAndScissor(commandBuffer, extent);

		auto& attachments = target.GetColorAttachments();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};

			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			commandBuffer.ImageMemoryBarrier(attachments[0]->GetView(), barrier);
		}

		compositionPass->Draw(commandBuffer, target);
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