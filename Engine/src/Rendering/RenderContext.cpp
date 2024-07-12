#include "RenderContext.hpp"

#include "Core/Window.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/Instance.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Image.hpp"

#include "RenderFrame.hpp"
#include "RenderTarget.hpp"

namespace Engine
{
    RenderContext::RenderContext(Window& window)
    {
        instance = std::make_unique<Vulkan::Instance>();

        surface = window.CreateSurface(*instance);

        physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, *surface);

        device = std::make_unique<Vulkan::Device>(*instance, *physicalDevice);
        
        auto size = window.GetFramebufferSize();

        swapchain = Vulkan::SwapchainBuilder()
            .DesiredWidth(size.width)
            .DesiredHeight(size.height)
            .MinImageCount(3)
            .Build(*device, *surface);

		CreateFrames();
    }

	RenderContext::~RenderContext()
	{
		device->WaitIdle();
	}

	void RenderContext::CreateFrames()
	{
		VkExtent2D extent = swapchain->GetImageExtent();
		VkFormat format = swapchain->GetImageFormat();

		for (auto handle : swapchain->GetImages())
		{
			auto swapchainImage = std::make_unique<Vulkan::Image>(
				*device,
				handle,
				swapchain->GetImageUsage(),
				format,
				VkExtent3D{ extent.width, extent.height, 1 }
			);

			auto target = CreateRenderTarget(std::move(swapchainImage));

			auto frame = std::make_unique<RenderFrame>(*device, std::move(target));

			frames.emplace_back(std::move(frame));
		}
	}

	Vulkan::CommandBuffer* RenderContext::Begin()
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

		return &commandBuffer;
	}

	void RenderContext::End(Vulkan::CommandBuffer& commandBuffer)
	{
		commandBuffer.EndRendering();

		auto& frame = GetCurrentFrame();
		auto& attachments = frame.GetTarget().GetColorAttachments();

		{
			Vulkan::ImageMemoryBarrierInfo barrier{};

			barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			barrier.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			barrier.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

			commandBuffer.ImageMemoryBarrier(attachments[0]->GetView(), barrier);
		}

		commandBuffer.End();

		Vulkan::Semaphore& waitSemaphore = Submit(commandBuffer);

		Present(waitSemaphore);

		acquireSemaphore = nullptr;
	}

	Vulkan::Semaphore& RenderContext::Submit(Vulkan::CommandBuffer& commandBuffer)
	{
		auto& frame = GetCurrentFrame();
		Vulkan::Semaphore& renderFinishedSemaphore = frame.RequestSemaphore();
		Vulkan::Fence& renderFence = frame.GetRenderFence();

		device->GetGraphicsQueue().Submit(commandBuffer, *acquireSemaphore, renderFinishedSemaphore, renderFence);

		return renderFinishedSemaphore;
	}

	void RenderContext::Present(Vulkan::Semaphore& waitSemaphore)
	{
		auto result = device->GetPresentQueue().Present(*swapchain, waitSemaphore, currentFrameIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	bool RenderContext::RecreateSwapchain(bool force)
	{
		auto details = device->GetSurfaceSupportDetails();

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

		device->WaitIdle();

		swapchain->Recreate(currentExtent.width, currentExtent.height);

		VkExtent2D extent = swapchain->GetImageExtent();
		VkFormat format = swapchain->GetImageFormat();

		auto it = frames.begin();

		for (auto& handle : swapchain->GetImages())
		{
			auto swapchainImage = std::make_unique<Vulkan::Image>(
				*device,
				handle,
				swapchain->GetImageUsage(),
				format,
				VkExtent3D{ extent.width, extent.height, 1 }
			);

			auto target = CreateRenderTarget(std::move(swapchainImage));

			(*it)->SetTarget(std::move(target));

			it++;
		}

		return true;
	}

	std::unique_ptr<RenderTarget> RenderContext::CreateRenderTarget(std::unique_ptr<Vulkan::Image>&& swapchainImage)
	{
		auto attachment = std::make_unique<RenderAttachment>(
			*device,
			std::move(swapchainImage),
			VkClearValue{
				.color = { 0.f, 0.f, 0.f, 1.f },
			},
			Vulkan::LoadStoreInfo{}
		);

		auto target = std::make_unique<RenderTarget>();

		target->AddColorAttachment(std::move(attachment));

		return std::move(target);
	}

	RenderFrame& RenderContext::GetCurrentFrame() const
	{
		return *frames[currentFrameIndex];
	}

	uint32_t RenderContext::GetCurrentFrameIndex() const
	{
		return currentFrameIndex;
	}

	uint32_t RenderContext::GetFrameCount() const
	{
		return frames.size();
	}

	Vulkan::Device& RenderContext::GetDevice()
	{
		return *device;
	}

	Vulkan::Instance& RenderContext::GetInstance()
	{
		return *instance;
	}

	Vulkan::PhysicalDevice& RenderContext::GetPhysicalDevice()
	{
		return *physicalDevice;
	}

	Vulkan::Swapchain& RenderContext::GetSwapchain()
	{
		return *swapchain;
	}
}