#pragma once

#include "Resource.h"

namespace Engine
{
	class RenderFrame;
}

namespace Vulkan
{
	class Device;
	class CommandBuffer;

	class CommandPool : public Resource<VkCommandPool>
	{
	public:
		CommandPool(Device &device, Engine::RenderFrame* frame = nullptr);
		~CommandPool();

		CommandBuffer& RequestCommandBuffer();
		Engine::RenderFrame* GetRenderFrame();

		void Reset();

	private:
		Device& device;
		Engine::RenderFrame* frame{ nullptr };

		std::vector<std::unique_ptr<CommandBuffer>> commandBuffers;

		uint32_t activeCommandBuffersCount = 0;
	};

}