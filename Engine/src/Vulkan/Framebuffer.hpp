#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#include "RenderPass.hpp"
#include "ImageView.hpp"
#include "Resource.hpp"

namespace Engine
{
	class RenderTarget;
}

namespace Vulkan
{
	class Device;

	class Framebuffer: public Resource<VkFramebuffer>
	{
	public:
		Framebuffer(const Device& device, const RenderPass& renderPass, const Engine::RenderTarget& target);
		~Framebuffer();

	private:
		const Device& device;

	};
}