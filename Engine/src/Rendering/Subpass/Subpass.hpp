#pragma once

#include "Rendering/Camera.hpp"

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/ShaderModule.hpp"

namespace Engine
{
	class Subpass
	{
	public:
		Subpass(Vulkan::ShaderSource&& vertexSource, Vulkan::ShaderSource&& fragmentSource);
		virtual ~Subpass() = default;

		virtual void Prepare(Vulkan::RenderPass& renderPass) {}
		virtual void SetCamera(const Camera& camera, const glm::mat4& transform) {}
		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) = 0;

		const Vulkan::ShaderSource& GetVertexShader() const;
		const Vulkan::ShaderSource& GetFragmentShader() const;

		const std::vector<uint32_t>& GetInputAttachments() const;
		void SetInputAttachments(std::vector<uint32_t> input);

		const std::vector<uint32_t>& GetOutputAttachments() const;
		void SetOutputAttachments(std::vector<uint32_t> output);

		const std::vector<uint32_t>& GetColorResolveAttachments() const;
		void SetColorResolveAttachments(std::vector<uint32_t> colorResolve);

		bool IsDepthStencilDisabled() const;
		void EnableDepthStencil();

	private:
		Vulkan::ShaderSource vertexShader;
		Vulkan::ShaderSource fragmentShader;

		std::vector<uint32_t> inputAttachments{};
		std::vector<uint32_t> outputAttachments{ 0 };
		std::vector<uint32_t> colorResolveAttachments{};

		bool disableDepthStencil = false;
	};
};