#include "Renderer.hpp"

#include <array>

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/ShaderModule.hpp"

#include "Vertex.hpp"
#include "Resource/ResourceManager.hpp"

namespace Engine
{

	void Renderer::Setup(Vulkan::Device& device, const Vulkan::Surface& surface, const Window& window)
	{
		auto size = window.GetFramebufferSize();

		auto swapchain = Vulkan::SwapchainBuilder()
			.DesiredWidth(size.width)
			.DesiredHeight(size.height)
			.MinImageCount(3)
			.Build(device, surface);

		Create(device, std::move(swapchain));
	}

	Renderer::Renderer(Vulkan::Device& device, std::unique_ptr<Vulkan::Swapchain> swapchain)
		: device(device), swapchain(std::move(swapchain))
	{
		renderPass = std::make_unique<Vulkan::RenderPass>(device, *this->swapchain);

		CreateFrames();
	}

	void Renderer::CreateFrames()
	{
		VkExtent2D extent = swapchain->GetImageExtent();
		VkFormat format = swapchain->GetImageFormat();

		for (auto image : swapchain->GetImages())
		{
			auto target = CreateTarget(image, format, extent);

			auto frame = std::make_unique<Frame>(device, std::move(target));

			frames.emplace_back(std::move(frame));
		}
	}


	Vulkan::CommandBuffer* Renderer::Begin()
	{
		auto& previousFrame = GetCurrentFrame();

		acquireSemaphore = &previousFrame.RequestSemaphore();

		auto result = swapchain->AcquireNextImageIndex(currentImageIndex, *acquireSemaphore);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			bool recreated = RecreateSwapchain(result == VK_ERROR_OUT_OF_DATE_KHR);

			if (!recreated)
			{
				return nullptr; // FIX: use a fence pool to prevent the renderer from freezing
			}

			result = swapchain->AcquireNextImageIndex(currentImageIndex, *acquireSemaphore);

			if (result != VK_SUCCESS)
			{
				previousFrame.Reset();
				return nullptr;
			}
		}

		auto& frame = GetCurrentFrame();
		frame.Reset();

		auto& commandBuffer = frame.RequestCommandBuffer();

		BeginRenderPass(commandBuffer);

		return &commandBuffer;
	}

	void Renderer::BeginRenderPass(Vulkan::CommandBuffer& commandBuffer)
	{
		commandBuffer.Begin();

		VkExtent2D extent = swapchain->GetImageExtent();


		commandBuffer.SetViewport({
			{
				.width = static_cast<float>(extent.width),
				.height = static_cast<float>(extent.height),
				.minDepth = 0.0f,
				.maxDepth = 1.0f,
			}
		});

		commandBuffer.SetScissor({
			{
				.extent = extent
			}
		});

		std::vector<VkClearValue> clearValues{ 2 };
		clearValues[0].color = { 0.f, 0.f, 0.f, 1.f };
		clearValues[1].depthStencil = { 1.f, 0 };

		auto& framebuffer = GetCurrentFrame().RequestFramebuffer(*renderPass);

		commandBuffer.BeginRenderPass(*renderPass, framebuffer, clearValues, extent);
	}

	void Renderer::End(Vulkan::CommandBuffer& commandBuffer)
	{
		commandBuffer.EndRenderPass();
		commandBuffer.End();

		Vulkan::Semaphore& waitSemaphore = Submit(commandBuffer);

		Present(waitSemaphore);

		acquireSemaphore = nullptr;
	}

	Vulkan::RenderPass& Renderer::GetRenderPass() const
	{
		return *renderPass;
	}

	Vulkan::Semaphore& Renderer::Submit(Vulkan::CommandBuffer& commandBuffer)
	{
		auto& frame = GetCurrentFrame();
		Vulkan::Semaphore& renderFinishedSemaphore = frame.RequestSemaphore();
		Vulkan::Fence& renderFence = frame.GetRenderFence();

		device.GetGraphicsQueue().Submit(commandBuffer, *acquireSemaphore, renderFinishedSemaphore, renderFence);

		return renderFinishedSemaphore;
	}

	void Renderer::Present(Vulkan::Semaphore& waitSemaphore)
	{
		auto result = device.GetPresentQueue().Present(*swapchain, waitSemaphore, currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	bool Renderer::RecreateSwapchain(bool force)
	{
		auto details = device.GetSurfaceSupportDetails();

		VkExtent2D currentExtent = details.capabilities.currentExtent;
		VkExtent2D swapchainExtent = swapchain->GetImageExtent();

		if (currentExtent.width == 0 || currentExtent.height == 0)
		{
			return false;
		}

		if (currentExtent.width == swapchainExtent.width && currentExtent.height == swapchainExtent.height && !force)
		{
			return false;
		}

		device.WaitIdle();

		swapchain->Recreate(currentExtent.width, currentExtent.height);

		VkExtent2D extent = swapchain->GetImageExtent();
		VkFormat format = swapchain->GetImageFormat();

		auto it = frames.begin();

		for (auto& image : swapchain->GetImages())
		{
			auto target = CreateTarget(image, format, extent);

			(*it)->SetTarget(std::move(target));

			it++;
		}

		return true;
	}

	Frame& Renderer::GetCurrentFrame() const
	{
		return *frames[currentImageIndex];
	}

	std::unique_ptr<Target> Renderer::CreateTarget(VkImage image, VkFormat format, VkExtent2D extent)
	{
		auto swapchainImage = std::make_unique<Vulkan::Image>(
			device,
			image,
			format,
			VkExtent3D{ extent.width, extent.height, 1 }
		);

		auto depthImage = std::make_unique<Vulkan::Image>(
			device,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_FORMAT_D32_SFLOAT,
			VkExtent3D{ extent.width, extent.height, 1 },
			device.GetMaxSampleCount()
		);

		auto colorImage = std::make_unique<Vulkan::Image>(
			device,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			format,
			VkExtent3D{ extent.width, extent.height, 1 },
			device.GetMaxSampleCount()
		);

		std::vector<std::unique_ptr<Vulkan::Image>> images;

		images.push_back(std::move(colorImage));
		images.push_back(std::move(depthImage));
		images.push_back(std::move(swapchainImage));

		return std::make_unique<Target>(device, std::move(images));
	}
}
