#include "Gui.hpp"

#include "Rendering/Renderer.hpp"

#include <imgui.h>
#include <ImGuizmo.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include <memory>

namespace Engine
{
    void Gui::Setup(Vulkan::Instance& instance, Vulkan::Device& device, Vulkan::PhysicalDevice& physicalDevice, Window& window, Vulkan::RenderPass& renderPass)
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

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForVulkan(static_cast<GLFWwindow*>(window.GetHandle()), true);

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = instance.GetHandle();
		initInfo.PhysicalDevice = physicalDevice.GetHandle();
		initInfo.Device = device.GetHandle();
		initInfo.Queue = device.GetGraphicsQueue().GetHandle();
		initInfo.DescriptorPool = descriptorPool->GetHandle();
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&initInfo, renderPass.GetHandle());

		device.OneTimeSubmit([](Vulkan::CommandBuffer& buffer) {
			ImGui_ImplVulkan_CreateFontsTexture(buffer.GetHandle());
		});

		device.ResetCommandPool();

		ImGui_ImplVulkan_DestroyFontUploadObjects();

		Create(std::move(descriptorPool), window);
    }

	void Gui::Begin()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	}

	void Gui::End(Vulkan::CommandBuffer& commandBuffer)
	{
		ImGui::Render();
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		auto [height, width] = window.GetFramebufferSize();

		io.DisplaySize = ImVec2((float)width, (float)height);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer.GetHandle());
	}

	Gui::Gui(std::unique_ptr<Vulkan::DescriptorPool> descriptorPool, Window& window)
		: descriptorPool(std::move(descriptorPool)), window(window) { }

	Gui::~Gui()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
};