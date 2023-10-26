#include "GeometrySubpass.hpp"

#include "Resource/ResourceManager.hpp"

namespace Engine
{
	GeometrySubpass::GeometrySubpass(
		Vulkan::Device& device,
		Vulkan::ShaderSource&& vertexSource,
		Vulkan::ShaderSource&& fragmentSource,
		Scene& scene
	) : device(device), Subpass{ std::move(vertexSource), std::move(fragmentSource) }, scene(scene)
	{
	}

	void GeometrySubpass::Prepare(Vulkan::RenderPass& renderPass)
	{
		PreparePipelineLayout();
		PreparePipeline(renderPass);
	}

	void GeometrySubpass::PreparePipelineLayout()
	{
		std::vector<Vulkan::ShaderModule> shaderModules
		{
			Vulkan::ShaderModule{ Vulkan::ShaderStage::Vertex, GetVertexShader()},
			Vulkan::ShaderModule{ Vulkan::ShaderStage::Fragment, GetFragmentShader() }
		};

		pipelineLayout = std::make_unique<Vulkan::PipelineLayout>(device, std::move(shaderModules));
	}

	void GeometrySubpass::PreparePipeline(Vulkan::RenderPass& renderPass)
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
					VkVertexInputAttributeDescription{
						.location = 1,
						.binding = 0,
						.format = VK_FORMAT_R32G32B32_SFLOAT,
						.offset = offsetof(Vertex, normal),
					},
					VkVertexInputAttributeDescription{
						.location = 2,
						.binding = 0,
						.format = VK_FORMAT_R32G32_SFLOAT,
						.offset = offsetof(Vertex, uv),
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

		pipeline = std::make_unique<Vulkan::Pipeline>(device, *pipelineLayout, renderPass, spec);
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

	void GeometrySubpass::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		commandBuffer.BindPipeline(*pipeline, VK_PIPELINE_BIND_POINT_GRAPHICS);

		UpdateGlobalUniform(commandBuffer);

		scene.ForEachEntity<Component::Transform, Component::MeshRender>(
			[&](Entity entity) {

				auto mesh = GetMeshFromEntity(entity);

				if (!mesh)
				{
					return;
				}

				glm::mat4 transform = GetEntityWorldMatrix(entity);

				for (auto& primitive : mesh->GetPrimitives())
				{
					auto material = GetMaterialFromPrimitive(*primitive);

					if (!material)
					{
						continue;
					}

					UpdateModelUniform(commandBuffer, *material, transform);

					primitive->Draw(commandBuffer);
				}
			}
		);
	}

	void GeometrySubpass::UpdateGlobalUniform(Vulkan::CommandBuffer& commandBuffer)
	{
		globalUniform.viewProjection = GetViewProjection();

		auto& frame = Renderer::Get().GetCurrentFrame();

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(GlobalUniform));

		allocation.SetData(&globalUniform);

		BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 0, 0);
	}

	glm::mat4 GeometrySubpass::GetViewProjection() const
	{
		auto [camera, transform] = Renderer::Get().GetMainCamera();

		return camera.GetProjection() * glm::inverse(transform);
	}

	std::shared_ptr<Mesh> GeometrySubpass::GetMeshFromEntity(Entity entity)
	{
		auto& meshRender = entity.GetComponent<Component::MeshRender>();

		return ResourceManager::Get().LoadResource<Mesh>(meshRender.mesh);
	}

	std::shared_ptr<Material> GeometrySubpass::GetMaterialFromPrimitive(const Primitive& primitive)
	{
		ResourceId materialId = primitive.GetMaterial();

		return ResourceManager::Get().LoadResource<Material>(materialId);
	}

	void GeometrySubpass::UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const Material& material, const glm::mat4& transform)
	{
		auto& frame = Renderer::Get().GetCurrentFrame();

		ModelUniform uniform{
			.model = transform,
			.color = material.GetColor(),
		};

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ModelUniform));

		allocation.SetData(&uniform);

		BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 1, 0);

		auto diffuse = ResourceManager::Get().LoadResource<Texture>(material.GetDiffuse());

		BindImage(diffuse->GetImageView(), diffuse->GetSampler(), 0, 2, 0);

		FlushDescriptorSet(commandBuffer, 0);
	}
}