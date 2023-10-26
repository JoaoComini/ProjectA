#pragma once

#include "GeometrySubpass.hpp"

#include <glm/glm.hpp>

namespace Engine
{
	struct LightUniform
	{
		glm::vec4 vector;
		glm::vec4 color;
	};

	class ForwardSubpass : public GeometrySubpass
	{
	public:
		ForwardSubpass(
			Vulkan::Device& device,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			Scene& scene
		);

		void Draw(Vulkan::CommandBuffer& commandBuffer) override;
	};
}