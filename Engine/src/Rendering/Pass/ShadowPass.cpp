#include "ShadowPass.hpp"

#include "Vulkan/ResourceCache.hpp"

#include "Resource/ResourceManager.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Mesh.hpp"

#include "Rendering/RenderGraph/RenderGraphCommand.hpp"

namespace Engine
{
    ShadowPass::ShadowPass(Scene& scene) : scene(scene) { }

	void ShadowPass::RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ShadowPassData& data)
	{
		data.shadowmap = builder.Allocate({
			.width = 2048,
			.height = 2048,
			.format = RenderTextureFormat::Depth,
			.usage = RenderTextureUsage::RenderTarget | RenderTextureUsage::Sampled
		});

		builder.Write(data.shadowmap, {
			.type = RenderTextureAccessType::Attachment,
			.attachment = {
				.aspect = RenderTextureAspect::Depth,
			}
		});

		auto query = scene.Query<Component::Transform, Component::DirectionalLight>();

		auto entity = query.First();

		if (scene.Valid(entity))
		{
			const auto& transform = scene.GetComponent<Component::Transform>(entity);

			auto direction = glm::normalize(transform.rotation * glm::vec3{ 0, 0, 1 });
		}

		context.Add<ShadowPassData>(data);
	}

	void ShadowPass::Render(RenderGraphCommand& command, const ShadowPassData& data)
	{
		command.DrawShadow(data.lightDirection);
	}
}
