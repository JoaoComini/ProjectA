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
        Scene& scene
    ) : Subpass{renderContext, std::move(vertexSource), std::move(fragmentSource)}, scene(scene)
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

		auto settings = Renderer::Get().GetSettings();

		auto [view, projection] = GetViewProjection();

		LightPushConstant pushConstant
		{
			.direction = -glm::normalize(glm::vec3(view[2])),
			.depthBias = -settings.shadow.depthBias * 50.f / 2048.f,
			.normalBias = -settings.shadow.normalBias * 50.f / 2048.f,
		};

		commandBuffer.PushConstants(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(LightPushConstant), &pushConstant);

		glm::mat4 viewProjection = projection * view;

		auto query = scene.Query<Component::MeshRender, Component::LocalToWorld>();

		for (auto entity : query)
		{
			auto& meshRender = query.GetComponent<Component::MeshRender>(entity);

			auto mesh = ResourceManager::Get().LoadResource<Mesh>(meshRender.mesh);

			glm::mat4 transform = query.GetComponent<Component::LocalToWorld>(entity).value;

			UpdateGlobalUniform(commandBuffer, transform, viewProjection);

			FlushDescriptorSet(commandBuffer, pipelineLayout, 0);

			for (auto& primitive : mesh->GetPrimitives())
			{
				ResourceId materialId = primitive->GetMaterial();

				auto material = ResourceManager::Get().LoadResource<Material>(materialId);

				if (material->GetAlphaMode() == AlphaMode::Blend)
				{
					continue;
				}

				primitive->Draw(commandBuffer);
			}
		}
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
		auto query = scene.Query<Component::Transform, Component::DirectionalLight>();

		auto entity = query.First();

		if (! scene.Valid(entity))
		{
			return {};
		}

		const auto& transform = scene.GetComponent<Component::Transform>(entity);

		auto direction = glm::normalize(transform.rotation * glm::vec3{ 0, 0, 1 });
		auto view = glm::lookAt(direction, glm::vec3{ 0 }, glm::vec3{0, 1, 0});

		auto projection = glm::ortho(-25.f, 25.f, -25.f, 25.f, -25.f, 25.f);

		return { view, projection };
    }
}
