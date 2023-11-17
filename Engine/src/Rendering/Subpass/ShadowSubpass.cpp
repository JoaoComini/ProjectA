#include "ShadowSubpass.hpp"

#include "Vulkan/Caching/ResourceCache.hpp"

namespace Engine
{
    ShadowSubpass::ShadowSubpass(
        Vulkan::Device& device,
        Vulkan::ShaderSource&& vertexSource,
        Vulkan::ShaderSource&& fragmentSource,
        Scene& scene,
		Camera& shadowCamera
    ) : GeometrySubpass{device, std::move(vertexSource), std::move(fragmentSource), scene}, shadowCamera(shadowCamera)
    {
    }

	void ShadowSubpass::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		auto [entity, found] = scene.FindFirstEntity<Component::Transform, Component::DirectionalLight>();

		if (!found)
		{
			light = Entity{};
			return;
		}

		light = entity;

		GeometrySubpass::Draw(commandBuffer);
	}

	Vulkan::PipelineLayout& ShadowSubpass::GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders)
	{
		return device.GetResourceCache().RequestPipelineLayout({ shaders[0] });
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
		if (!light)
		{
			return {};
		}

		const auto& transform = light.GetComponent<Component::Transform>();

		auto projection = shadowCamera.GetProjection();
		auto view = glm::inverse(transform.GetLocalMatrix());

		return { view, projection };
    }
}
