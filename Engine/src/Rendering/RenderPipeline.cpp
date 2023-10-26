#include "RenderPipeline.hpp"

#include "Subpass/ForwardSubpass.hpp"
#include "Subpass/ShadowSubpass.hpp"

namespace Engine
{
    RenderPipeline::RenderPipeline(Vulkan::Device& device, Scene& scene)
		: device(device), scene(scene)
    {
		shadowCamera = std::make_unique<OrthographicCamera>(-200.f, 200.f, -200.f, 200.f, 120.f, -120.f);
		SetupMainPass();
		SetupShadowPass();
    }

	void RenderPipeline::SetupMainPass()
	{
		auto vertexSource = Vulkan::ShaderSource{ "resources/shaders/forward.vert.spv" };
		auto fragmentSource = Vulkan::ShaderSource{ "resources/shaders/forward.frag.spv" };

		auto forwardSubpass = std::make_unique<ForwardSubpass>(
			device,
			std::move(vertexSource),
			std::move(fragmentSource),
			scene,
			*shadowCamera,
			shadowPassTargets
		);

		forwardSubpass->SetColorResolveAttachments({ 0 });
		forwardSubpass->SetOutputAttachments({ 2 });
		forwardSubpass->SetSampleCount(device.GetMaxSampleCount());

		std::vector<std::unique_ptr<Subpass>> subpasses;
		subpasses.push_back(std::move(forwardSubpass));

		mainPass = std::make_unique<Pass>(device, std::move(subpasses));

		std::vector<Vulkan::LoadStoreInfo> loadStoreInfos
		{
			Vulkan::LoadStoreInfo{
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			},
			Vulkan::LoadStoreInfo{
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			},
			Vulkan::LoadStoreInfo{
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			}
		};

		mainPass->SetLoadStoreInfos(loadStoreInfos);
		mainPass->Prepare(Renderer::Get().GetCurrentFrame().GetTarget());
	}

	void RenderPipeline::SetupShadowPass()
	{
		for (size_t i = 0; i < Renderer::Get().GetFrameCount(); i++)
		{
			shadowPassTargets.push_back(CreateShadowPassTarget());
		}

		auto vertexSource = Vulkan::ShaderSource{ "resources/shaders/shadowmap.vert.spv" };
		auto fragmentSource = Vulkan::ShaderSource{ "resources/shaders/shadowmap.frag.spv" };

		auto subpass = std::make_unique<ShadowSubpass>(
			device,
			std::move(vertexSource),
			std::move(fragmentSource),
			scene,
			*shadowCamera
		);

		std::vector<std::unique_ptr<Subpass>> subpasses;
		subpasses.push_back(std::move(subpass));

		std::vector<VkClearValue> clearValues
		{
			{
				.depthStencil = { 1.f, 0 }
			}
		};

		shadowPass = std::make_unique<Pass>(device, std::move(subpasses));

		shadowPass->SetClearValues(clearValues);
		shadowPass->Prepare(*shadowPassTargets[0]);
	}

	std::unique_ptr<RenderTarget> RenderPipeline::CreateShadowPassTarget()
	{
		auto depthImage = std::make_unique<Vulkan::Image>(
			device,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_FORMAT_D32_SFLOAT,
			VkExtent3D{ 2048, 2048, 1 }
		);

		std::vector<std::unique_ptr<Vulkan::Image>> images;

		images.push_back(std::move(depthImage));

		return std::make_unique<RenderTarget>(device, std::move(images));
	}

	void RenderPipeline::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		DrawShadowPass(commandBuffer);
		DrawMainPass(commandBuffer);
	}

	void RenderPipeline::DrawShadowPass(Vulkan::CommandBuffer& commandBuffer)
	{
		auto& target = shadowPassTargets[Renderer::Get().GetCurrentFrameIndex()];

		VkExtent2D extent = target->GetExtent();

		SetViewportAndScissor(commandBuffer, extent);
		
		{
			Vulkan::ImageMemoryBarrierInfo barrier{};
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

			commandBuffer.ImageMemoryBarrier(*target->GetViews()[0], barrier);
		}

		shadowPass->Draw(commandBuffer, *target);

		commandBuffer.EndRenderPass();
	}

	void RenderPipeline::DrawMainPass(Vulkan::CommandBuffer& commandBuffer)
	{
		auto& target = Renderer::Get().GetCurrentFrame().GetTarget();

		VkExtent2D extent = target.GetExtent();

		SetViewportAndScissor(commandBuffer, extent);

		{
			auto& shadowTarget = shadowPassTargets[Renderer::Get().GetCurrentFrameIndex()];
			auto& shadowMap = shadowTarget->GetViews()[0];

			Vulkan::ImageMemoryBarrierInfo barrier{};
			barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

			commandBuffer.ImageMemoryBarrier(*shadowMap, barrier);
		}

		mainPass->Draw(commandBuffer, target);
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

	Vulkan::RenderPass& RenderPipeline::GetMainRenderPass() const
	{
		return mainPass->GetRenderPass();
	}
}