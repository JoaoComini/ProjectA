#pragma once

#include "Subpass.hpp"

#include "Scene/Scene.hpp"
#include "Rendering/Cubemap.hpp"
#include "Rendering/Mesh.hpp"

namespace Engine
{
	class SkyboxSubpass : public Subpass
	{
	public:
		SkyboxSubpass(
			Vulkan::Device& device,
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