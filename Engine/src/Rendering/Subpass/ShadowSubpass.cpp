#include "ShadowSubpass.hpp"

#include "Vulkan/Caching/ResourceCache.hpp"

#include "Resource/ResourceManager.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Mesh.hpp"

namespace Engine
{
    ShadowSubpass::ShadowSubpass(
        RenderContext& renderContext,
        Vulkan::ShaderSource&& vertexSource,
        Vulkan::ShaderSource&& fragmentSource,
        Scene& scene,
		Camera& shadowCamera
    ) : Subpass{renderContext, std::move(vertexSource), std::move(fragmentSource)}, scene(scene), shadowCamera(shadowCamera)
    {
    }

	Vulkan::PipelineLayout& ShadowSubpass::GetPipelineLayout(const std::vector<Vulkan::ShaderModule*>& shaders)
	{
		return GetRenderContext().GetDevice().GetResourceCache().RequestPipelineLayout({ shaders[0] });
	}

	struct LightPushConstant
	{
		glm::vec3 direction;
		float depthBias;
		float normalBias;
	};

	void ShadowSubpass::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		auto& resourceCache = GetRenderContext().GetDevice().GetResourceCache();

		auto& vertexShader = resourceCache.RequestShaderModule(VK_SHADER_STAGE_VERTEX_BIT, GetVertexShader(), {});
		auto& fragmentShader = resourceCache.RequestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, GetFragmentShader(), {});

		auto& pipelineLayout = GetPipelineLayout({ &vertexShader, &fragmentShader });

		commandBuffer.BindPipelineLayout(pipelineLayout);

		Vulkan::VertexInputState vertexInputState{};

		vertexInputState.attributes.resize(2);
		vertexInputState.attributes[0].location = 0;
		vertexInputState.attributes[0].binding = 0;
		vertexInputState.attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputState.attributes[0].offset = offsetof(Vertex, position);

		vertexInputState.attributes[1].location = 1;
		vertexInputState.attributes[1].binding = 0;
		vertexInputState.attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputState.attributes[1].offset = offsetof(Vertex, normal);

		vertexInputState.bindings.resize(1);
		vertexInputState.bindings[0].binding = 0;
		vertexInputState.bindings[0].stride = sizeof(Vertex);
		vertexInputState.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		commandBuffer.SetVertexInputState(vertexInputState);

		Vulkan::RasterizationState rasterizationState{
			.cullMode = VK_CULL_MODE_NONE,
		};

		commandBuffer.SetRasterizationState(rasterizationState);

		auto settings = Renderer::Get().GetSettings();

		auto [view, projection] = GetViewProjection();

		LightPushConstant pushConstant
		{
			.direction = -glm::normalize(glm::vec3(view[2])),
			.depthBias = -settings.shadow.depthBias * 50.f/2048.f,
			.normalBias = -settings.shadow.normalBias * 50.f / 2048.f,
		};

		commandBuffer.PushConstants(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(LightPushConstant), &pushConstant);

		glm::mat4 viewProjection = projection * view;

		scene.ForEachEntity<Component::MeshRender>(
			[&](Entity entity) {
				auto& meshRender = entity.GetComponent<Component::MeshRender>();

				auto mesh = ResourceManager::Get().LoadResource<Mesh>(meshRender.mesh);

				if (!mesh)
				{
					return;
				}

				glm::mat4 transform = entity.GetComponent<Component::LocalToWorld>().value;

				UpdateGlobalUniform(commandBuffer, transform, viewProjection);

				FlushDescriptorSet(commandBuffer, pipelineLayout, 0);

				for (auto& primitive : mesh->GetPrimitives())
				{
					primitive->Draw(commandBuffer);
				}
			}
		);
	}

	struct GlobalUniform
	{
		glm::mat4 model;
		glm::mat4 viewProjection;
	};

	void ShadowSubpass::UpdateGlobalUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& transform, const glm::mat4& viewProjection)
	{
		GlobalUniform uniform{};
		uniform.model = transform;
		uniform.viewProjection = viewProjection;

		auto& frame = GetRenderContext().GetCurrentFrame();

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(GlobalUniform));

		allocation.SetData(&uniform);

		BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 0, 0);
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
