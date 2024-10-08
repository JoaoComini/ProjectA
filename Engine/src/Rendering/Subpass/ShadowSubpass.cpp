#include "ShadowSubpass.hpp"

#include "Vulkan/Caching/ResourceCache.hpp"

#include "Rendering/Renderer.hpp"

namespace Engine
{
    ShadowSubpass::ShadowSubpass(
        RenderContext& renderContext,
        Vulkan::ShaderSource&& vertexSource,
        Vulkan::ShaderSource&& fragmentSource,
        Scene& scene,
		Camera& shadowCamera
    ) : GeometrySubpass{renderContext, std::move(vertexSource), std::move(fragmentSource), scene}, shadowCamera(shadowCamera)
    {
    }

	Vulkan::PipelineLayout& ShadowSubpass::GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders)
	{
		return GetRenderContext().GetDevice().GetResourceCache().RequestPipelineLayout({ shaders[0] });
	}

	void ShadowSubpass::PreparePipelineState(Vulkan::CommandBuffer& commandBuffer)
    {
		Vulkan::VertexInputState vertexInputState{};

		vertexInputState.attributes.resize(1);
		vertexInputState.attributes[0].location = 0;
		vertexInputState.attributes[0].binding = 0;
		vertexInputState.attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputState.attributes[0].offset = offsetof(Vertex, position);

		vertexInputState.bindings.resize(1);
		vertexInputState.bindings[0].binding = 0;
		vertexInputState.bindings[0].stride = sizeof(Vertex);
		vertexInputState.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		commandBuffer.SetVertexInputState(vertexInputState);
    }

	std::pair<glm::mat4, glm::mat4> ShadowSubpass::GetViewProjection() const
    {
		auto entity = scene.FindFirstEntity<Component::Transform, Component::DirectionalLight>();

		if (!entity)
		{
			return {};
		}

		const auto& transform = entity.GetComponent<Component::Transform>();

		auto projection = shadowCamera.GetProjection();
		auto view = glm::inverse(transform.GetLocalMatrix());

		return { view, projection };
    }
}
