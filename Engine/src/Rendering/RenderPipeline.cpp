#include "RenderPipeline.hpp"

#include <Shaders/embed.gen.hpp>

#include "Scene/Scene.hpp"

#include "Vulkan/Swapchain.hpp"
#include "RenderGraph/RenderGraph.hpp"
#include "RenderGraph/VulkanRenderGraphAllocator.hpp"
#include "RenderGraph/VulkanRenderGraphCommand.hpp"

#include "RenderBatcher.hpp"
#include "RenderFrame.hpp"

namespace Engine
{
    RenderPipeline::RenderPipeline(RenderContext& renderContext, Scene& scene)
		: renderContext(renderContext), scene(scene)
    {
		allocator = std::make_unique<VulkanRenderGraphAllocator>(renderContext.GetDevice());

		shadowPass = std::make_unique<ShadowPass>(scene);
		compositionPass = std::make_unique<CompositionPass>();
		mainPass = std::make_unique<ForwardPass>(scene);

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
		point.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;

		VkSamplerCreateInfo shadow{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
		shadow.minFilter = VK_FILTER_LINEAR;
		shadow.magFilter = VK_FILTER_LINEAR;
		shadow.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
		shadow.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadow.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadow.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		shadow.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		shadow.compareEnable = VK_TRUE;
		shadow.compareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;

		samplers.emplace(RenderTextureSampler::Point, std::make_unique<Vulkan::Sampler>(renderContext.GetDevice(), point));
		samplers.emplace(RenderTextureSampler::Shadow, std::make_unique<Vulkan::Sampler>(renderContext.GetDevice(), shadow));
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
	}
}