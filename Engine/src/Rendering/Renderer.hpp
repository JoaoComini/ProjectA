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
#include "Frame.hpp"

namespace Engine
{

	struct GlobalUniform
	{
		glm::mat4 viewProjection;
	};

	struct ModelUniform
	{
		glm::mat4 model;
		glm::vec4 color;
	};

	class Renderer : public Singleton<Renderer>
	{
	public:
		Renderer(Vulkan::Device& device, std::unique_ptr<Vulkan::Swapchain> swapchain);

		static void Setup(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window);

		Vulkan::CommandBuffer* Begin();

		void SetCamera(const Camera& camera, const glm::mat4& transform);

		void Draw(const Mesh& mesh, const glm::mat4& transform);
		void End();

		Vulkan::RenderPass& GetRenderPass() const;

	private:

		void CreateDescriptors();
		void CreatePipeline();
		void CreateFrames();
		void CreateFramebuffers();

		void BeginCommandBuffer();
		void UpdateGlobalUniform();
		Vulkan::Semaphore& Submit();
		void Present(Vulkan::Semaphore& waitSemaphore);
		bool RecreateSwapchain(bool force = false);

		Frame& GetCurrentFrame() const;
		std::unique_ptr<Target> CreateTarget(VkImage image, VkFormat format, VkExtent2D extent);

		const Vulkan::Device& device;

		GlobalUniform globalUniform;
		uint32_t currentImageIndex = 0;

		std::unique_ptr<Vulkan::Swapchain> swapchain;
		Vulkan::Semaphore* acquireSemaphore;
		Vulkan::CommandBuffer* activeCommandBuffer;

		std::vector<std::unique_ptr<Frame>> frames;
		std::vector<std::unique_ptr<Vulkan::Framebuffer>> framebuffers;

		std::unique_ptr<Vulkan::Pipeline> pipeline;
		std::unique_ptr<Vulkan::PipelineLayout> pipelineLayout;

		std::unique_ptr<Vulkan::RenderPass> renderPass;
		std::shared_ptr<Vulkan::DescriptorSetLayout> globalDescriptorSetLayout;
		std::shared_ptr<Vulkan::DescriptorSetLayout> modelDescriptorSetLayout;
	};
}
