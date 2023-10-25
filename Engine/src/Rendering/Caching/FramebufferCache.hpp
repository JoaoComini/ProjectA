#pragma once

#include "Vulkan/Framebuffer.hpp"
#include "../RenderTarget.hpp"

#include <unordered_map>

namespace Engine
{
	class FramebufferCache
	{
	public:
		FramebufferCache(const Vulkan::Device& device);

		Vulkan::Framebuffer& RequestFramebuffer(const Vulkan::RenderPass& renderPass, const RenderTarget& target);
		void Clear();

	private:
		std::unordered_map<std::size_t, std::unique_ptr<Vulkan::Framebuffer>> framebuffers;

		const Vulkan::Device& device;
	};
}