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

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Material.hpp"
#include "Camera.hpp"
#include "Frame.hpp"

namespace Rendering
{
	class Renderer
	{
	public:
		Renderer(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window);
		~Renderer();

		void Begin(Camera& camera);
		void Draw(Mesh& mesh, glm::mat4 transform);
		void End();

	private:
		void CreateDescriptors();
		void CreatePipeline();
		void CreateSampler();
		void CreateFrames();
		void CreateFramebuffers();

		void BeginCommandBuffer();
		Vulkan::Semaphore& Submit();
		void Present(Vulkan::Semaphore& waitSemaphore);
		bool RecreateSwapchain(bool force = false);

	private:
		uint32_t currentImageIndex = 0;

		const Vulkan::Device& device;
		const Vulkan::Surface& surface;

		Camera *camera;

		std::unique_ptr<Vulkan::Swapchain> swapchain;
		Vulkan::Semaphore* acquireSemaphore;
		Vulkan::CommandBuffer* activeCommandBuffer;

		std::vector<std::unique_ptr<Frame>> frames;
		std::vector<std::unique_ptr<Vulkan::Framebuffer>> framebuffers;

		std::unique_ptr<Vulkan::Pipeline> pipeline;
		std::unique_ptr<Vulkan::PipelineLayout> pipelineLayout;

		std::unique_ptr<Vulkan::RenderPass> renderPass;

		VkDescriptorPool descriptorPool;
		VkDescriptorSetLayout descriptorSetLayout;
		VkSampler sampler;

		std::unique_ptr<Texture> texture;
	};
}
