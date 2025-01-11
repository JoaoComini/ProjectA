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
		float alphaCutoff;
	};

	struct SelectedPrimitive
	{
		glm::mat4 transform;
		Primitive& primitive;
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
		void UpdateCameraUniform(Vulkan::CommandBuffer& commandBuffer, CameraUniform& uniform);
		void UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& matrix, const Material& material);

		void SelectPrimitivesToRender(std::vector<SelectedPrimitive>& opaques, std::multimap<float, SelectedPrimitive>& transparents, glm::vec3& cameraPosition);

		void DrawOpaques(Vulkan::CommandBuffer& commandBuffer, std::vector<SelectedPrimitive>& opaques);
		void DrawTransparents(Vulkan::CommandBuffer& commandBuffer, std::multimap<float, SelectedPrimitive>& opaques);

		std::shared_ptr<Material> GetMaterialFromPrimitive(const Primitive& primitive);

		std::vector<Vulkan::ShaderModule*> shaders;
	};

}