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

	Vulkan::PipelineLayout& ShadowSubpass::GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders)
	{
		return device.GetResourceCache().RequestPipelineLayout({ shaders[0] });
	}

	Vulkan::Pipeline& ShadowSubpass::GetPipeline(Vulkan::PipelineLayout& pipelineLayout, Vulkan::PipelineSpec& spec)
    {
		spec.vertexInput.attributes.resize(1);
		spec.vertexInput.attributes[0].location = 0;
		spec.vertexInput.attributes[0].binding = 0;
		spec.vertexInput.attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		spec.vertexInput.attributes[0].offset = offsetof(Vertex, position);

		spec.vertexInput.bindings.resize(1);
		spec.vertexInput.bindings[0].binding = 0;
		spec.vertexInput.bindings[0].stride = sizeof(Vertex);
		spec.vertexInput.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		spec.multisample.rasterizationSamples = sampleCount;

		return device.GetResourceCache().RequestPipeline(pipelineLayout, *renderPass, spec);
    }

	std::pair<glm::mat4, glm::mat4> ShadowSubpass::GetViewProjection() const
    {
		auto [entity, found] = scene.FindFirstEntity<Component::Transform, Component::DirectionalLight>();

		if (found)
		{
			const auto& transform = entity.GetComponent<Component::Transform>();

			auto projection = shadowCamera.GetProjection();
			auto view = glm::inverse(transform.GetLocalMatrix());

			return { view, projection };
		}

		return {};
    }
}
