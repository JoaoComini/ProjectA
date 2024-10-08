#include "GeometrySubpass.hpp"

#include "Resource/ResourceManager.hpp"

#include "Vulkan/Caching/ResourceCache.hpp"
#include "Rendering/Renderer.hpp"

namespace Engine
{
	GeometrySubpass::GeometrySubpass(
		RenderContext& renderContext,
		Vulkan::ShaderSource&& vertexSource,
		Vulkan::ShaderSource&& fragmentSource,
		Scene& scene
	) : Subpass{ renderContext, std::move(vertexSource), std::move(fragmentSource) }, scene(scene)
	{
	}

	void GeometrySubpass::PreparePipelineState(Vulkan::CommandBuffer& commandBuffer)
	{
		Vulkan::VertexInputState vertexInputState{};

		vertexInputState.attributes.resize(3);
		vertexInputState.attributes[0].location = 0;
		vertexInputState.attributes[0].binding = 0;
		vertexInputState.attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputState.attributes[0].offset = offsetof(Vertex, position);

		vertexInputState.attributes[1].location = 1;
		vertexInputState.attributes[1].binding = 0;
		vertexInputState.attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputState.attributes[1].offset = offsetof(Vertex, normal);

		vertexInputState.attributes[2].location = 2;
		vertexInputState.attributes[2].binding = 0;
		vertexInputState.attributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexInputState.attributes[2].offset = offsetof(Vertex, uv);

		vertexInputState.bindings.resize(1);
		vertexInputState.bindings[0].binding = 0;
		vertexInputState.bindings[0].stride = sizeof(Vertex);
		vertexInputState.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		commandBuffer.SetVertexInputState(vertexInputState);
	}

	glm::mat4 GetEntityWorldMatrix(Entity entity)
	{
		auto parent = entity.GetParent();
		auto& transform = entity.GetComponent<Component::Transform>();

		if (!parent)
		{
			return transform.GetLocalMatrix();
		}

		auto parentTransform = parent.TryGetComponent<Component::Transform>();

		if (!parentTransform)
		{
			return transform.GetLocalMatrix();
		}

		return GetEntityWorldMatrix(parent) * transform.GetLocalMatrix();
	}

	std::shared_ptr<Mesh> GetMeshFromEntity(Entity entity)
	{
		auto& meshRender = entity.GetComponent<Component::MeshRender>();

		return ResourceManager::Get().LoadResource<Mesh>(meshRender.mesh);
	}

	void GeometrySubpass::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		PreparePipelineState(commandBuffer);

		scene.ForEachEntity<Component::MeshRender>(
			[&](Entity entity) {

				glm::mat4 transform = entity.GetComponent<Component::LocalToWorld>().value;

				UpdateGlobalUniform(commandBuffer, transform);

				auto mesh = GetMeshFromEntity(entity);

				if (! mesh)
				{
					return;
				}

				for (auto& primitive : mesh->GetPrimitives())
				{
					auto material = GetMaterialFromPrimitive(*primitive);

					if (!material)
					{
						continue;
					}

					UpdateModelUniform(commandBuffer, *material);

					primitive->Draw(commandBuffer);
				}
			}
		);
	}

	void GeometrySubpass::UpdateGlobalUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& transform)
	{
		auto [view, projection] = GetViewProjection();

		GlobalUniform uniform{};
		uniform.model = transform;
		uniform.viewProjection = projection * view;
		uniform.cameraPosition = glm::inverse(view)[3];

		auto& frame = GetRenderContext().GetCurrentFrame();

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(GlobalUniform));

		allocation.SetData(&uniform);

		BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 0, 0);
	}

	std::pair<glm::mat4, glm::mat4> GeometrySubpass::GetViewProjection() const
	{
		auto [camera, transform] = Renderer::Get().GetMainCamera();

		return { glm::inverse(transform), camera.GetProjection() };
	}

	std::shared_ptr<Material> GeometrySubpass::GetMaterialFromPrimitive(const Primitive& primitive)
	{
		ResourceId materialId = primitive.GetMaterial();

		return ResourceManager::Get().LoadResource<Material>(materialId);
	}

	void GeometrySubpass::UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const Material& material)
	{
		auto& frame = GetRenderContext().GetCurrentFrame();

		if (auto albedo = ResourceManager::Get().LoadResource<Texture>(material.GetAlbedoTexture()))
		{
			BindImage(albedo->GetImageView(), albedo->GetSampler(), 0, 1, 0);
		}

		if (auto normal = ResourceManager::Get().LoadResource<Texture>(material.GetNormalTexture()))
		{
			BindImage(normal->GetImageView(), normal->GetSampler(), 0, 2, 0);
		}

		if (auto metallicRoughness = ResourceManager::Get().LoadResource<Texture>(material.GetMetallicRoughnessTexture()))
		{
			BindImage(metallicRoughness->GetImageView(), metallicRoughness->GetSampler(), 0, 3, 0);
		}

		auto& resourceCache = GetRenderContext().GetDevice().GetResourceCache();

		auto& variant = material.GetShaderVariant();

		shaders.clear();

		auto& vertexShader = resourceCache.RequestShaderModule(VK_SHADER_STAGE_VERTEX_BIT, GetVertexShader(), variant);
		auto& fragmentShader = resourceCache.RequestShaderModule(VK_SHADER_STAGE_FRAGMENT_BIT, GetFragmentShader(), variant);

		shaders.push_back(&vertexShader);
		shaders.push_back(&fragmentShader);

		auto& pipelineLayout = GetPipelineLayout(shaders);

		commandBuffer.BindPipelineLayout(pipelineLayout);

		if (pipelineLayout.HasShaderResource(Vulkan::ShaderResourceType::PushConstant))
		{
			PbrPushConstant pushConstant
			{
				.albedoColor = material.GetAlbedoColor(),
				.metallicFactor = material.GetMetallicFactor(),
				.roughnessFactor = material.GetRoughnessFactor(),
			};

			commandBuffer.PushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PbrPushConstant), &pushConstant);
		}

		FlushDescriptorSet(commandBuffer, pipelineLayout, 0);
	}
}