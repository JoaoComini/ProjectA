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
		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) override;

	protected:
		Scene& scene;

	private:
		void UpdateGlobalUniform(Vulkan::CommandBuffer& commandBuffer);
		std::shared_ptr<Mesh> GetMeshFromEntity(Entity entity);
		std::shared_ptr<Material> GetMaterialFromPrimitive(const Primitive& primitive);
		void UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const Material& material, const glm::mat4& transform);

		GlobalUniform globalUniform{};

		Vulkan::Device& device;
	};

}