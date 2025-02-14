#include "ShadowPass.hpp"


#include "Resource/ResourceManager.hpp"
#include "Rendering/Renderer.hpp"

#include "Rendering/RenderGraphCommand.hpp"

namespace Engine
{
    ShadowPass::ShadowPass(Scene& scene, ShadowSettings settings) : scene(scene), settings(settings) { }

	void ShadowPass::RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ShadowPassData& data)
	{
		data.shadowmap = builder.Allocate<RenderTexture>({
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

		const auto query = scene.Query<Component::Transform, Component::DirectionalLight>();

		if (const auto entity = query.First(); scene.Valid(entity))
		{
			const auto& transform = scene.GetComponent<Component::Transform>(entity);

			data.lightDirection = glm::normalize(transform.rotation * glm::vec3{ 0, 0, 1 });
		}

		context.Add<ShadowPassData>(data);
	}

	void ShadowPass::Render(RenderGraphCommand& command, const ShadowPassData& data)
	{
		command.DrawShadow(data.lightDirection);
	}
}
