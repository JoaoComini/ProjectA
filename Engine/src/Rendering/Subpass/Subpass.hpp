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

		virtual void SetCamera(const Camera& camera, const glm::mat4& transform) {}
		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) = 0;

		const Vulkan::ShaderSource& GetVertexShader() const;
		const Vulkan::ShaderSource& GetFragmentShader() const;

	private:

		Vulkan::ShaderSource vertexShader;
		Vulkan::ShaderSource fragmentShader;
	};
};