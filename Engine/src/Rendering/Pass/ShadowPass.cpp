#include "ShadowPass.h"


#include "Resource/ResourceManager.h"
#include "Rendering/Renderer.h"

#include "Rendering/RenderGraphCommand.h"

namespace Engine
{
    ShadowPass::ShadowPass(SceneGraph& scene, ShadowSettings settings) : scene(scene), settings(settings) { }

	void ShadowPass::RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, ShadowPassData& data)
	{
		data.shadowMap = builder.Allocate<RenderTexture>({
			.width = 2048,
			.height = 2048,
			.format = RenderTextureFormat::Depth,
			.usage = RenderTextureUsage::RenderTarget | RenderTextureUsage::Sampled
		});

		builder.Write(data.shadowMap, {
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
		command.DrawShadow({
			.lightDirection = data.lightDirection,
			.depthBias = settings.depthBias,
			.normalBias = settings.normalBias,
		});
	}
}
