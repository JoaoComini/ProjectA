#include "RenderPipeline.hpp"

#include "Subpass/GeometrySubpass.hpp"

namespace Engine
{
    RenderPipeline::RenderPipeline(Vulkan::Device& device, Scene& scene)
    {
		auto vertexSource = Vulkan::ShaderSource{ "resources/shaders/shader.vert.spv" };
		auto fragmentSource = Vulkan::ShaderSource{ "resources/shaders/shader.frag.spv" };

		auto geometrySubpass = std::make_unique<GeometrySubpass>(device, std::move(vertexSource), std::move(fragmentSource), scene);

		geometrySubpass->SetColorResolveAttachments({ 0 });
		geometrySubpass->SetOutputAttachments({ 2 });

		std::vector<std::unique_ptr<Subpass>> subpasses;
		subpasses.push_back(std::move(geometrySubpass));

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

	void RenderPipeline::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		mainPass->Draw(commandBuffer);
	}

	Vulkan::RenderPass& RenderPipeline::GetMainRenderPass() const
	{
		return mainPass->GetRenderPass();
	}
}