#include "GeometryPass.hpp"

#include "Resource/ResourceManager.hpp"
#include "Rendering/Renderer.hpp"

#include "Vulkan/ResourceCache.hpp"

namespace Engine
{
	GeometryPass::GeometryPass(
		RenderContext& renderContext,
		ShaderSource&& vertexSource,
		ShaderSource&& fragmentSource,
		Scene& scene
	) : Pass{ renderContext, std::move(vertexSource), std::move(fragmentSource) }, scene(scene)
	{
	}

	void GeometryPass::PreparePipelineState(Vulkan::CommandBuffer& commandBuffer)
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

	void GeometryPass::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		PreparePipelineState(commandBuffer);

		auto [view, projection] = GetViewProjection();

		CameraUniform uniform{};
		uniform.viewProjectionMatrix = projection * view;
		uniform.position = glm::inverse(view)[3];

		UpdateCameraUniform(commandBuffer, uniform);

		std::multimap<uint64_t, SelectedPrimitive> opaques;
		std::multimap<float, SelectedPrimitive> transparents;

		SelectPrimitivesToRender(opaques, transparents, uniform.position);

		DrawOpaques(commandBuffer, opaques);
		DrawTransparents(commandBuffer, transparents);
	}

	void GeometryPass::UpdateCameraUniform(Vulkan::CommandBuffer& commandBuffer, CameraUniform& uniform)
	{
		auto& frame = GetRenderContext().GetCurrentFrame();

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(CameraUniform));

		allocation.SetData(&uniform);

		commandBuffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 0, 0);
	}

	std::pair<glm::mat4, glm::mat4> GeometryPass::GetViewProjection() const
	{
		auto [camera, transform] = Renderer::Get().GetMainCamera();

		return { glm::inverse(transform), camera.GetProjection() };
	}

	void GeometryPass::SelectPrimitivesToRender(std::multimap<std::size_t, SelectedPrimitive>& opaques, std::multimap<float, SelectedPrimitive>& transparents, glm::vec3& cameraPosition)
	{
		auto query = scene.Query<Component::MeshRender, Component::LocalToWorld>();

		for (auto entity : query)
		{
			auto [meshRender, localToWorld] = query.GetComponent(entity);

			auto mesh = ResourceManager::Get().LoadResource<Mesh>(meshRender.mesh);

			for (auto& primitive : mesh->GetPrimitives())
			{
				auto material = GetMaterialFromPrimitive(*primitive);

				auto bounds = mesh->GetBounds();
				bounds.Transform(localToWorld.value);

				auto distance = glm::length2(bounds.GetCenter() - cameraPosition);

				if (material->GetAlphaMode() == AlphaMode::Blend)
				{
					transparents.emplace(distance, SelectedPrimitive{
						.transform = localToWorld.value,
						.primitive = *primitive,
						.material = *material,
					});

					continue;
				}
				
				std::size_t hash{ 0 };
				Hash(hash, *material);
				opaques.emplace(hash, SelectedPrimitive{
					.transform = localToWorld.value,
					.primitive = *primitive,
					.material = *material,
				});
			}
		}
	}

	void GeometryPass::DrawOpaques(Vulkan::CommandBuffer& commandBuffer, std::multimap<uint64_t, SelectedPrimitive>& opaques)
	{
		for (auto opaque = opaques.begin(); opaque != opaques.end(); opaque++)
		{
			auto& sorted = opaque->second;

			auto material = GetMaterialFromPrimitive(sorted.primitive);

			UpdateModelUniform(commandBuffer, sorted.transform, *material);

			sorted.primitive.Draw(commandBuffer);
		}
	}

	void GeometryPass::DrawTransparents(Vulkan::CommandBuffer& commandBuffer, std::multimap<float, SelectedPrimitive>& transparents)
	{
		Vulkan::ColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.blendEnable = VK_TRUE;

		Vulkan::ColorBlendState colorBlendState{};
		colorBlendState.attachments.push_back(colorBlendAttachment);

		commandBuffer.SetColorBlendState(colorBlendState);

		// Draw transparent objects back to front
		for (auto transparent = transparents.rbegin(); transparent != transparents.rend(); transparent++)
		{
			auto& sorted = transparent->second;

			auto material = GetMaterialFromPrimitive(sorted.primitive);

			UpdateModelUniform(commandBuffer, sorted.transform, *material);

			sorted.primitive.Draw(commandBuffer);
		}
	}

	std::shared_ptr<Material> GeometryPass::GetMaterialFromPrimitive(const Primitive& primitive)
	{
		ResourceId materialId = primitive.GetMaterial();

		return ResourceManager::Get().LoadResource<Material>(materialId);
	}

	void GeometryPass::UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& matrix, const Material& material)
	{
		auto& frame = GetRenderContext().GetCurrentFrame();

		ModelUniform uniform{};
		uniform.localToWorldMatrix = matrix;

		auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ModelUniform));
		allocation.SetData(&uniform);

		commandBuffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 1, 0);

		if (auto albedo = ResourceManager::Get().LoadResource<Texture>(material.GetAlbedoTexture()))
		{
			commandBuffer.BindImage(albedo->GetImageView(), albedo->GetSampler(), 0, 2, 0);
		}

		if (auto normal = ResourceManager::Get().LoadResource<Texture>(material.GetNormalTexture()))
		{
			commandBuffer.BindImage(normal->GetImageView(), normal->GetSampler(), 0, 3, 0);
		}

		if (auto metallicRoughness = ResourceManager::Get().LoadResource<Texture>(material.GetMetallicRoughnessTexture()))
		{
			commandBuffer.BindImage(metallicRoughness->GetImageView(), metallicRoughness->GetSampler(), 0, 4, 0);
		}

		auto& resourceCache = GetRenderContext().GetDevice().GetResourceCache();

		auto& variant = material.GetShaderVariant();

		shaders.clear();

		auto& vertexShader = resourceCache.RequestShader(ShaderStage::Vertex , GetVertexShader(), variant);
		auto& fragmentShader = resourceCache.RequestShader(ShaderStage::Fragment, GetFragmentShader(), variant);

		shaders.push_back(&vertexShader);
		shaders.push_back(&fragmentShader);

		auto& pipelineLayout = GetPipelineLayout(shaders);

		commandBuffer.BindPipelineLayout(pipelineLayout);

		if (pipelineLayout.HasShaderResource(ShaderResourceType::PushConstant))
		{
			PbrPushConstant pushConstant
			{
				.albedoColor = material.GetAlbedoColor(),
				.metallicFactor = material.GetMetallicFactor(),
				.roughnessFactor = material.GetRoughnessFactor(),
				.alphaCutoff = material.GetAlphaCutoff(),
			};

			commandBuffer.PushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PbrPushConstant), &pushConstant);
		}

		commandBuffer.FlushDescriptorSet(0);
	}
}