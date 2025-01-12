#pragma once

#include "Pass.hpp"

namespace Engine
{
	class Scene;
	class Mesh;

	class SkyboxPass : public Pass
	{
	public:
		SkyboxPass(
			RenderContext& renderContext,
			Vulkan::ShaderSource&& vertexSource,
			Vulkan::ShaderSource&& fragmentSource,
			Scene &scene
		);

		virtual void Draw(Vulkan::CommandBuffer& commandBuffer) override;
	private:
		std::shared_ptr<Mesh> cube;

		Scene& scene;
	};
}