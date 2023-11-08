#pragma once

#include "Subpass.hpp"
#include "Scene/Scene.hpp"


namespace Engine
{
	struct alignas(16) GlobalUniform
	{
		glm::mat4 model;
		glm::mat4 viewProjection;
		glm::vec3 cameraPosition;
	};

	struct ModelUniform
	{
		glm::vec4 color;
	};

	struct PbrPushConstant
	{
		glm::vec4 albedoColor;
		float metallicFactor;
		float roughnessFactor;
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

		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) override;

	protected:
		virtual void PreparePipelineState(Vulkan::CommandBuffer& commandBuffer);

		virtual std::pair<glm::mat4, glm::mat4> GetViewProjection() const;

		Scene& scene;
	private:
		void UpdateGlobalUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& transform);
		std::shared_ptr<Mesh> GetMeshFromEntity(Entity entity);
		std::shared_ptr<Material> GetMaterialFromPrimitive(const Primitive& primitive);
		void UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const Material& material);

		std::vector<Vulkan::ShaderModule*> shaders;
	};

}