#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <tiny_obj_loader.h>

#include <stdexcept>
#include <vector>
#include <string.h>
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <iostream>

#include "Vulkan/Instance.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Device.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/Semaphore.hpp"
#include "Vulkan/Semaphore.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/Frame.hpp"

#include "Window.hpp"
#include "WindowSurface.hpp"

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class Application
{
public:
	void Run()
	{
		Init();
		MainLoop();
	}

private:
	std::unique_ptr<Window> window;
	std::unique_ptr<Vulkan::Instance> instance;
	std::unique_ptr<WindowSurface> surface;
	Vulkan::PhysicalDevice physicalDevice;
	std::unique_ptr<Vulkan::Device> device;
	std::unique_ptr<Vulkan::Swapchain> swapchain;

	std::unique_ptr<Renderer> renderer;

	std::vector<std::unique_ptr<Rendering::Frame>> frames;

	uint32_t currentImageIndex = 0;
	bool windowHasResized = false;

	void Init()
	{
		window = WindowBuilder()
			.Height(HEIGHT)
			.Width(WIDTH)
			.UserPointer(this)
			.Build();

		window->SetFramebufferSizeCallback(
			[](Window* window, FramebufferSize size)
			{
				auto app = window->GetUserPointer<Application>();
				app->windowHasResized = true;
			}
		);

		instance = Vulkan::InstanceBuilder().Build();

		surface = std::make_unique<WindowSurface>(*instance, window->GetHandle());

		physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, surface->GetHandle());

		device = std::make_unique<Vulkan::Device>(*instance, physicalDevice);

		auto size = window->GetFramebufferSize();

		swapchain = Vulkan::SwapchainBuilder()
			.DesiredWidth(size.width)
			.DesiredHeight(size.height)
			.MinImageCount(3)
			.Build(*device, *surface);

		renderer = std::make_unique<Renderer>(*device, *swapchain);

		CreateFrames();
	}

	void CreateFrames()
	{
		for (size_t i = 0; i < swapchain->GetImageCount(); i++)
		{
			frames.emplace_back(std::make_unique<Rendering::Frame>(*device));
		}
	}

	void MainLoop()
	{
		while (!window->ShouldClose())
		{
			window->Update();
			Render();
		}

		device->WaitIdle();
	}

	void Render()
	{
		Vulkan::Semaphore& acquireSemaphore = frames[currentImageIndex]->RequestSemaphore();

		currentImageIndex = swapchain->AcquireNextImageIndex(acquireSemaphore);

		frames[currentImageIndex]->Reset();

		Vulkan::Semaphore& renderFinishedSemaphore = frames[currentImageIndex]->RequestSemaphore();
		Vulkan::Fence& renderFence = frames[currentImageIndex]->GetRenderFence();

		Vulkan::CommandBuffer& commandBuffer = frames[currentImageIndex]->RequestCommandBuffer();

		renderer->Render(commandBuffer, currentImageIndex);

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphore.GetHandle() };
		VkSemaphore waitSemaphores[] = { acquireSemaphore.GetHandle() };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer.GetHandle();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(device->GetGraphicsQueue().GetHandle(), 1, &submitInfo, renderFence.GetHandle()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapchains[] = { swapchain->GetHandle() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &currentImageIndex;
		presentInfo.pResults = nullptr; // Optional

		auto result = vkQueuePresentKHR(device->GetPresentQueue().GetHandle(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || windowHasResized)
		{
			windowHasResized = false;
			RecreateSwapchain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	void RecreateSwapchain()
	{
		window->WaitForFocus();
		device->WaitIdle();

		renderer->ResetImages();

		auto size = window->GetFramebufferSize();
		swapchain->Recreate(size.width, size.height);

		renderer->CreateImages();
	}
};