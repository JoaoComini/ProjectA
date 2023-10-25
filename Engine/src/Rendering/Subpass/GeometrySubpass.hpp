#pragma once

#include "Subpass.hpp"
#include "Scene/Scene.hpp"


namespace Engine
{
	struct GlobalUniform
	{
		glm::mat4 viewProjection;
	};

	struct ModelUniform
	{
		glm::mat4 model;
		glm::vec4 color;
	};

	class GeometrySubpass : public Subpass
	{
	public:
		GeometrySubpass(
			Vulkan::Device& device,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			Scene& scene
		);

		void Prepare(Vulkan::RenderPass& renderPass) override;
		void Draw(Vulkan::CommandBuffer& commandBuffer) override;

	private:
		void UpdateGlobalUniform(Vulkan::CommandBuffer& commandBuffer);
		std::shared_ptr<Mesh> GetMeshFromEntity(Entity entity);
		std::shared_ptr<Material> GetMaterialFromPrimitive(const Primitive& primitive);
		void UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const Material& material, const glm::mat4& transform);

		Scene& scene;

		GlobalUniform globalUniform{};

		std::unique_ptr<Vulkan::Pipeline> pipeline;
		std::unique_ptr<Vulkan::PipelineLayout> pipelineLayout;

		Vulkan::Device& device;
	};

}