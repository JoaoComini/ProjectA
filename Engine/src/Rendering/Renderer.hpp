#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

#include "Core/Window.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Image.hpp"
#include "Vulkan/ImageView.hpp"
#include "Vulkan/Buffer.hpp"
#include "Vulkan/Framebuffer.hpp"
#include "Vulkan/Pipeline.hpp"
#include "Vulkan/PipelineLayout.hpp"
#include "Vulkan/RenderPass.hpp"
#include "Vulkan/DescriptorSetLayout.hpp"

#include "Common/Singleton.hpp"

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "RenderFrame.hpp"

namespace Engine
{
	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer(Vulkan::Device& device, std::unique_ptr<Vulkan::Swapchain> swapchain);

		static void Setup(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window);

		Vulkan::CommandBuffer* Begin();
		void End(Vulkan::CommandBuffer& commandBuffer);

		void SetMainCamera(Camera& camera, glm::mat4& transform);

		std::pair<Camera&, glm::mat4&> GetMainCamera();

		RenderFrame& GetCurrentFrame() const;
	private:
		void CreateFrames();

		void BeginRenderPass(Vulkan::CommandBuffer& commandBuffer);

		Vulkan::Semaphore& Submit(Vulkan::CommandBuffer& commandBuffer);

		void Present(Vulkan::Semaphore& waitSemaphore);

		bool RecreateSwapchain(bool force = false);

		std::unique_ptr<RenderTarget> CreateTarget(std::unique_ptr<Vulkan::Image> swapchainImage);

		uint32_t currentFrameIndex = 0;

		std::unique_ptr<Vulkan::Swapchain> swapchain;
		Vulkan::Semaphore* acquireSemaphore;

		std::vector<std::unique_ptr<RenderFrame>> frames;

		Camera mainCamera;
		glm::mat4 mainCameraTransform;

		const Vulkan::Device& device;
	};
}
