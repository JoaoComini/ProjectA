#include "RenderPipeline.hpp"

#include <Shaders/embed.gen.hpp>

#include "Scene/Scene.hpp"

#include "Vulkan/Swapchain.hpp"
#include "RenderGraph/RenderGraph.hpp"
#include "RenderGraph/VulkanRenderGraphAllocator.hpp"
#include "RenderGraph/VulkanRenderGraphCommand.hpp"

#include "RenderBatcher.hpp"

namespace Engine
{
    RenderPipeline::RenderPipeline(RenderContext& renderContext, Scene& scene)
		: renderContext(renderContext), scene(scene)
    {
		allocator = std::make_unique<VulkanRenderGraphAllocator>(renderContext.GetDevice());

		SetupShadowPass();
		SetupGBufferPass();
		SetupCompositionPass();

		VkSamplerCreateInfo point{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		point.magFilter = VK_FILTER_NEAREST;
		point.minFilter = VK_FILTER_NEAREST;
		point.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		point.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		point.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		point.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		point.mipLodBias = 0.0f;
		point.minLod = 0.0f;
		point.maxLod = 1.0f;
		point.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

		VkSamplerCreateInfo shadow{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		shadow.minFilter = VK_FILTER_LINEAR;
		shadow.magFilter = VK_FILTER_LINEAR;
		point.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		shadow.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadow.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadow.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		shadow.compareEnable = VK_TRUE;
		shadow.compareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;

		samplers.emplace(RenderTextureSampler::Point, std::make_unique<Vulkan::Sampler>(renderContext.GetDevice(), point));
		samplers.emplace(RenderTextureSampler::Shadow, std::make_unique<Vulkan::Sampler>(renderContext.GetDevice(), shadow));
    }

	void RenderPipeline::SetupGBufferPass()
	{
		gBufferTarget = CreateGBufferPassTarget();

		auto forwardVert = embed::Shaders::get("forward.vert.glsl");
		auto forwardFrag = embed::Shaders::get("forward.frag.glsl");

		auto forwardSubpass = std::make_unique<ForwardPass>(
			renderContext,
			ShaderSource{ std::vector<uint8_t>{ forwardVert.begin(), forwardVert.end() }},
			ShaderSource{ std::vector<uint8_t>{ forwardFrag.begin(), forwardFrag.end() } },
			scene,
			shadowTarget.get()
		);

		//auto skyboxVert = embed::Shaders::get("skybox.vert.glsl");
		//auto skyboxFrag = embed::Shaders::get("skybox.frag.glsl");

		//auto skyboxSubpass = std::make_unique<SkyboxPass>(
		//	renderContext,
		//	ShaderSource{ std::vector<uint8_t>{ skyboxVert.begin(), skyboxVert.end() }},
		//	ShaderSource{ std::vector<uint8_t>{ skyboxFrag.begin(), skyboxFrag.end() } },
		//	scene
		//);

		mainPass = std::make_unique<ForwardPass>(
			renderContext,
			ShaderSource{ std::vector<uint8_t>{ forwardVert.begin(), forwardVert.end() } },
			ShaderSource{ std::vector<uint8_t>{ forwardFrag.begin(), forwardFrag.end() } },
			scene,
			shadowTarget.get()
		);
	}

	std::unique_ptr<RenderTarget> RenderPipeline::CreateGBufferPassTarget()
	{
		auto extent = renderContext.GetSwapchain().GetImageExtent();

		auto colorAttachment = RenderAttachment::Builder(renderContext.GetDevice())
			.Usage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
			.Format(VK_FORMAT_R16G16B16A16_SFLOAT)
			.Extent(extent)
			.LoadStoreInfo({
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			})
			.Build();

		auto depthAttachment = RenderAttachment::Builder(renderContext.GetDevice())
			.Usage(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
			.Format(renderContext.GetPhysicalDevice().GetSupportedDepthFormat())
			.Extent(extent)
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

		auto vertexSource = ShaderSource{ std::vector<uint8_t>{ shadowmapVert.begin(), shadowmapVert.end() }};
		auto fragmentSource = ShaderSource{ std::vector<uint8_t>{ shadowmapFrag.begin(), shadowmapFrag.end() }};

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

		auto vertexSource = ShaderSource{ std::vector<uint8_t>{ compositionVert.begin(), compositionVert.end() } };
		auto fragmentSource = ShaderSource{ std::vector<uint8_t>{ compositionFrag.begin(), compositionFrag.end() } };

		compositionPass = std::make_unique<CompositionPass>(
			renderContext,
			std::move(vertexSource),
			std::move(fragmentSource),
			gBufferTarget.get()
		);
	}

	void RenderPipeline::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		auto& target = renderContext.GetCurrentFrame().GetTarget();

		RenderGraph graph;
		RenderBatcher batcher{ scene };
		batcher.BuildBatches();

		auto& context = graph.GetContext();
		auto& data = context.Add<BackbufferData>();

		data.backbuffer = graph.Import({
			.width = target.GetExtent().width,
			.height = target.GetExtent().height,
			.format = RenderTextureFormat::Linear,
			.usage = RenderTextureUsage::RenderTarget | RenderTextureUsage::Sampled
		}, { target.GetColorAttachmentPtr(0) });

		graph.AddPass(*shadowPass);
		graph.AddPass(*mainPass);
		graph.AddPass(*compositionPass);

		graph.Compile();

		VulkanRenderGraphCommand command{ renderContext, batcher, shaderCache, commandBuffer, samplers };
		graph.Execute(command, *allocator);

		//DrawShadowPass(commandBuffer);
		//DrawMainPass(commandBuffer);
		//DrawCompositionPass(commandBuffer);
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