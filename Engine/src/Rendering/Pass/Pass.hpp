#pragma once

#include "Rendering/Subpass/Subpass.hpp"
#include "Rendering/RenderTarget.hpp"

#include <vector>

namespace Engine
{

	class Pass
	{
	public:
		Pass(Vulkan::Device& device, std::vector<std::unique_ptr<Subpass>>&& subpasses);

		void Draw(Vulkan::CommandBuffer& commandBuffer, RenderTarget& renderTarget);
	private:
		std::vector<std::unique_ptr<Subpass>> subpasses;

		Vulkan::Device& device;
	};
}