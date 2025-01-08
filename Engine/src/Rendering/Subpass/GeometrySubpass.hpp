#pragma once

#include "Subpass.hpp"
#include "Scene/Scene.hpp"

namespace Engine
{
	struct CameraUniform
	{
		glm::mat4 viewProjectionMatrix;
		glm::vec3 position;
	};

	struct ModelUniform
	{
		glm::mat4 localToWorldMatrix;
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
			RenderContext& renderContext,
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
		void UpdateCameraUniform(Vulkan::CommandBuffer& commandBuffer);
		std::shared_ptr<Material> GetMaterialFromPrimitive(const Primitive& primitive);
		void UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& matrix, const Material& material);

		std::vector<Vulkan::ShaderModule*> shaders;
	};

}