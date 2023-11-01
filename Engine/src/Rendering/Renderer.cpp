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
		CreateFrames();
	}

	void Renderer::CreateFrames()
	{
		VkExtent2D extent = swapchain->GetImageExtent();
		VkFormat format = swapchain->GetImageFormat();

		for (auto handle : swapchain->GetImages())
		{
			auto swapchainImage = std::make_unique<Vulkan::Image>(
				device,
				handle,
				swapchain->GetImageUsage(),
				format,
				VkExtent3D{ extent.width, extent.height, 1 }
			);

			auto target = CreateTarget(std::move(swapchainImage));

			auto frame = std::make_unique<RenderFrame>(device, std::move(target));

			frames.emplace_back(std::move(frame));
		}
	}


	Vulkan::CommandBuffer* Renderer::Begin()
	{
		auto& previousFrame = GetCurrentFrame();

		acquireSemaphore = &previousFrame.RequestSemaphore();

		auto result = swapchain->AcquireNextImageIndex(currentFrameIndex, *acquireSemaphore);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			bool recreated = RecreateSwapchain(result == VK_ERROR_OUT_OF_DATE_KHR);

			if (!recreated)
			{
				return nullptr; // FIX: use a fence pool to prevent the renderer from freezing
			}

			result = swapchain->AcquireNextImageIndex(currentFrameIndex, *acquireSemaphore);

			if (result != VK_SUCCESS)
			{
				previousFrame.Reset();
				return nullptr;
			}
		}

		auto& frame = GetCurrentFrame();
		frame.Reset();

		auto& commandBuffer = frame.RequestCommandBuffer();

		commandBuffer.Begin();

		auto& target = Renderer::Get().GetCurrentFrame().GetTarget();

		auto& views = target.GetViews();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};

			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

			commandBuffer.ImageMemoryBarrier(*views[0], barrier);
		}

		return &commandBuffer;
	}

	void Renderer::End(Vulkan::CommandBuffer& commandBuffer)
	{
		commandBuffer.EndRenderPass();

		auto& frame = GetCurrentFrame();
		auto& views = frame.GetTarget().GetViews();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};

			barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

			commandBuffer.ImageMemoryBarrier(*views[0], barrier);
		}

		commandBuffer.End();

		Vulkan::Semaphore& waitSemaphore = Submit(commandBuffer);

		Present(waitSemaphore);

		acquireSemaphore = nullptr;
	}

	void Renderer::SetMainCamera(Camera& camera, glm::mat4 transform)
	{
		mainCamera = &camera;
		mainCameraTransform = std::move(transform);
	}

	std::pair<Camera&, glm::mat4&> Renderer::GetMainCamera()
	{
		return { *mainCamera, mainCameraTransform };
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
		auto result = device.GetPresentQueue().Present(*swapchain, waitSemaphore, currentFrameIndex);

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

		for (auto& handle : swapchain->GetImages())
		{
			auto swapchainImage = std::make_unique<Vulkan::Image>(
				device,
				handle,
				swapchain->GetImageUsage(),
				format,
				VkExtent3D{ extent.width, extent.height, 1 }
			);

			auto target = CreateTarget(std::move(swapchainImage));

			(*it)->SetTarget(std::move(target));

			it++;
		}

		return true;
	}

	RenderFrame& Renderer::GetCurrentFrame() const
	{
		return *frames[currentFrameIndex];
	}

	uint32_t Renderer::GetCurrentFrameIndex() const
	{
		return currentFrameIndex;
	}

	uint32_t Renderer::GetFrameCount() const
	{
		return frames.size();
	}

	void Renderer::SetSettings(RendererSettings settings)
	{
		this->settings = settings;
	}

	RendererSettings Renderer::GetSettings() const
	{
		return settings;
	}

	std::unique_ptr<RenderTarget> Renderer::CreateTarget(std::unique_ptr<Vulkan::Image> swapchainImage)
	{
		std::vector<std::unique_ptr<Vulkan::Image>> images;

		images.push_back(std::move(swapchainImage));

		return std::make_unique<RenderTarget>(device, std::move(images));
	}
}
