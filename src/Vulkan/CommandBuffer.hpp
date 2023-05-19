#pragma once

#include <vulkan/vulkan.h>
 
#include "Resource.hpp"

namespace Vulkan
{
	class CommandPool;
	class Device;

	class CommandBuffer : public Resource<VkCommandBuffer>
	{
	public:
		enum class Level
		{
			Primary,
			Secondary
		};

		enum class BeginFlags
		{
			None = 0x0,
			OneTimeSubmit = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
		};

		CommandBuffer(const Device& device, const CommandPool& commandPool, const Level level = Level::Primary);
		~CommandBuffer();

		void Begin(BeginFlags flags = BeginFlags::None);
		void End();
		void Free();
		void CopyBuffer(VkBuffer src, VkBuffer dst, uint32_t size);
		void CopyBufferToImage(VkBuffer src, VkImage dst, uint32_t width, uint32_t height);
		void SetImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout);

	private:
		const Device& device;
		const CommandPool& commandPool;
	};
}
