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

	Vulkan::PipelineLayout& ShadowSubpass::GetPipelineLayout(const std::vector<Vulkan::ShaderModule>& shaders)
	{
		return device.GetResourceCache().RequestPipelineLayout({ shaders[0] });
	}

	Vulkan::Pipeline& ShadowSubpass::GetPipeline(Vulkan::PipelineLayout& pipelineLayout)
    {
		auto spec = Vulkan::PipelineSpec
		{
			.vertexInput
			{
				.attributes = {
					VkVertexInputAttributeDescription{
						.location = 0,
						.binding = 0,
						.format = VK_FORMAT_R32G32B32_SFLOAT,
						.offset = offsetof(Vertex, position),
					},
				},
				.bindings = {
					VkVertexInputBindingDescription{
						.binding = 0,
						.stride = sizeof(Vertex),
						.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
					}
				}
			},
			.multisample
			{
				.rasterizationSamples = sampleCount
			}
		};

		return device.GetResourceCache().RequestPipeline(pipelineLayout, *renderPass, spec);
    }

    glm::mat4 ShadowSubpass::GetViewProjection() const
    {
		auto [entity, found] = scene.FindFirstEntity<Component::Transform, Component::DirectionalLight>();

		if (found)
		{
			const auto& transform = entity.GetComponent<Component::Transform>();

			auto projection = shadowCamera.GetProjection();
			auto view = glm::inverse(transform.GetLocalMatrix());

			return projection * view;
		}

		return glm::mat4{ 1.f };
    }
}
