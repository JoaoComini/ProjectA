#include "GeometrySubpass.hpp"

#include "Resource/ResourceManager.hpp"

namespace Engine
{
	GeometrySubpass::GeometrySubpass(
		Vulkan::Device& device,
		Vulkan::ShaderSource&& vertexSource,
		Vulkan::ShaderSource&& fragmentSource,
		Scene& scene
	) : Subpass{std::move(vertexSource), std::move(fragmentSource)}, scene(scene)
	{
		std::vector<std::shared_ptr<Vulkan::ShaderModule>> shaderModules
		{
			std::make_shared<Vulkan::ShaderModule>(device, Vulkan::ShaderStage::Vertex, GetVertexShader()),
			std::make_shared<Vulkan::ShaderModule>(device, Vulkan::ShaderStage::Fragment, GetFragmentShader())
		};

		pipelineLayout = std::make_unique<Vulkan::PipelineLayout>(
			device,
			shaderModules
		);

		auto spec = Vulkan::PipelineSpec
		{
			.vertexInputSpec
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
			.shaderModules = shaderModules
		};

		auto& renderPass = Renderer::Get().GetRenderPass();
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

	void GeometrySubpass::SetCamera(const Camera& camera, const glm::mat4 transform)
	{
		globalUniform.viewProjection = camera.GetProjection() * glm::inverse(transform);
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
		auto& frame = Renderer::Get().GetCurrentFrame();

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(GlobalUniform));

		allocation.SetData(&globalUniform);

		std::vector<SetBinding<VkDescriptorBufferInfo>> bufferInfos
		{
			{
				.binding = 0,
				.infos = {
					VkDescriptorBufferInfo{
						.buffer = allocation.GetBuffer().GetHandle(),
						.offset = allocation.GetOffset(),
						.range = allocation.GetSize(),
					}
				}
			}
		};

		auto& descritorSetLayout = pipelineLayout->GetDescriptorSetLayout(0);

		auto descriptorSet = frame.RequestDescriptorSet(descritorSetLayout, bufferInfos, {});

		commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, 0, descriptorSet);
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

		std::vector<SetBinding<VkDescriptorBufferInfo>> bufferInfos
		{
			{
				.binding = 0,
				.infos = {
					VkDescriptorBufferInfo{
						.buffer = allocation.GetBuffer().GetHandle(),
						.offset = allocation.GetOffset(),
						.range = allocation.GetSize(),
					}
				}
			}
		};

		auto diffuse = ResourceManager::Get().LoadResource<Texture>(material.GetDiffuse());


		std::vector<SetBinding<VkDescriptorImageInfo>> imageInfos
		{
			{
				.binding = 1,
				.infos = {
					VkDescriptorImageInfo{
						.sampler = diffuse->GetSampler().GetHandle(),
						.imageView = diffuse->GetImageView().GetHandle(),
						.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					}
				}
			}
		};

		auto& descritorSetLayout = pipelineLayout->GetDescriptorSetLayout(1);

		auto descriptorSet = frame.RequestDescriptorSet(descritorSetLayout, bufferInfos, imageInfos);

		commandBuffer.BindDescriptorSet(VK_PIPELINE_BIND_POINT_GRAPHICS, *pipelineLayout, 1, descriptorSet);
	}
}