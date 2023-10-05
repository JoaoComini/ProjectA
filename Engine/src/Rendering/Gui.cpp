#include "Gui.hpp"

#include "Rendering/Renderer.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <memory>

namespace Engine
{
    Gui* Gui::Setup(Vulkan::Instance& instance, Vulkan::Device& device, Vulkan::PhysicalDevice& physicalDevice, Window& window)
    {
		if (Gui::instance == nullptr)
		{

			std::vector<VkDescriptorPoolSize> poolSizes =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};

			std::unique_ptr<Vulkan::DescriptorPool> descriptorPool = std::make_unique<Vulkan::DescriptorPool>(device, poolSizes, 1000);

			ImGui::CreateContext();
			ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow *>(window.GetHandle()), true);

			ImGui_ImplVulkan_InitInfo initInfo = {};
			initInfo.Instance = instance.GetHandle();
			initInfo.PhysicalDevice = physicalDevice.GetHandle();
			initInfo.Device = device.GetHandle();
			initInfo.Queue = device.GetGraphicsQueue().GetHandle();
			initInfo.DescriptorPool = descriptorPool->GetHandle();
			initInfo.MinImageCount = 3;
			initInfo.ImageCount = 3;
			initInfo.MSAASamples = device.GetMaxSampleCount();

			ImGui_ImplVulkan_Init(&initInfo, Renderer::GetInstance()->GetRenderPass().GetHandle());

			device.OneTimeSubmit([&](Vulkan::CommandBuffer& buffer) {
				ImGui_ImplVulkan_CreateFontsTexture(buffer.GetHandle());
			});

			device.ResetCommandPool();

			ImGui_ImplVulkan_DestroyFontUploadObjects();

			Gui::instance = new Gui(std::move(descriptorPool));
		}

		return Gui::instance;
    }

	void Gui::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Gui::End(Vulkan::CommandBuffer& commandBuffer)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetHandle());
	}

	Gui::Gui(std::unique_ptr<Vulkan::DescriptorPool> descriptorPool)
		: descriptorPool(std::move(descriptorPool)) { }

	Gui::~Gui()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
};