#include "Renderer.hpp"

#include "Vulkan/CommandBuffer.hpp"

#include "Core/Window.hpp"

#include "Resource/ResourceManager.hpp"
#include "RenderContext.hpp"

#include "RenderGraph/RenderGraph.hpp"

#include "VulkanRenderGraphAllocator.hpp"
#include "VulkanRenderGraphCommand.hpp"

namespace Engine
{
	Renderer::Renderer(RenderContext& renderContext)
		: renderContext(renderContext)
	{
		allocator = std::make_unique<VulkanRenderGraphAllocator>(renderContext.GetDevice());

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

	Renderer::~Renderer()
	{
		allocator.reset();
		samplers.clear();
	}

	void Renderer::Draw(Vulkan::CommandBuffer& commandBuffer, Scene& scene, RenderCamera& camera, RenderAttachment& target)
	{
		RenderGraph graph;
		RenderBatcher batcher;
		batcher.BuildBatches(scene, camera);

		auto& graphContext = graph.GetContext();
		graphContext.Add<ShadowSettings>(settings.shadow);
		graphContext.Add<ResolutionSettings>(settings.resolution);

		auto& backbufferData = graphContext.Add<BackbufferData>();

		backbufferData.target = graph.Import<RenderTexture>(
			{ &target },
			{
				.width = target.GetExtent().width,
				.height = target.GetExtent().height,
				.format = RenderTextureFormat::Linear,
				.usage = RenderTextureUsage::RenderTarget
			}
		);

		ShadowPass shadowPass{ scene, {} };
		ForwardPass forwardPass{ scene };
		CompositionPass compositionPass;

		graph.AddPass(shadowPass);
		graph.AddPass(forwardPass);
		graph.AddPass(compositionPass);

		graph.Compile();

		VulkanRenderGraphCommand command{ renderContext, batcher, shaderCache, commandBuffer, samplers };
		graph.Execute(command, *allocator);
	}
}
