#include "Gui.hpp"

#include "Vulkan/DescriptorPool.hpp"
#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/Swapchain.hpp"
#include "Vulkan/ResourceCache.hpp"

#include "Rendering/Renderer.hpp"
#include "Rendering/RenderFrame.hpp"
#include "Rendering/RenderContext.hpp"
#include "Rendering/RenderPipeline.hpp"

#include "Core/Window.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <ImGuizmo.h>

namespace Engine
{
    Gui::Gui(Window& window)
		: window(window)
    {
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOther(static_cast<GLFWwindow*>(window.GetHandle()), true);

		UploadFonts(io);
		LoadShaders();
    }

	Gui::~Gui()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Gui::UploadFonts(ImGuiIO& io)
	{
		auto& device = Renderer::Get().GetRenderContext().GetDevice();

		uint8_t* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		size_t size = width * height * 4 * sizeof(char);

		std::vector<uint8_t> bytes{ pixels, pixels + size };
		std::vector<Mipmap> mipmaps{
			{
				.level = 0,
				.offset = 0,
				.extent = {
					.width = static_cast<uint32_t>(width),
					.height = static_cast<uint32_t>(height),
					.depth = 1
				}
			}
		};

		fontTexture = std::make_unique<Texture>(std::move(bytes), std::move(mipmaps));
		fontTexture->CreateVulkanResources(device);
		fontTexture->UploadDataToGpu(device);
	}

	void Gui::LoadShaders()
	{
		auto& device = Renderer::Get().GetRenderContext().GetDevice();

		auto vertexBytes = embed::Shaders::get("imgui.vert.glsl");
		auto fragBytes = embed::Shaders::get("imgui.frag.glsl");

		auto vertexSource = ShaderSource{ std::vector<uint8_t>{ vertexBytes.begin(), vertexBytes.end() } };
		auto fragSource = ShaderSource{ std::vector<uint8_t>{ fragBytes.begin(), fragBytes.end() } };

		std::vector<Shader*> shaders;
		shaders.push_back(&device.GetResourceCache().RequestShader(ShaderStage::Vertex, vertexSource, {}));
		shaders.push_back(&device.GetResourceCache().RequestShader(ShaderStage::Fragment, fragSource, {}));

		pipelineLayout = &device.GetResourceCache().RequestPipelineLayout(shaders);
	}


	void Gui::Begin()
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
	}

	void Gui::Draw(Vulkan::CommandBuffer& commandBuffer)
	{
		ImGui::Render();

		auto& frame = Renderer::Get().GetRenderContext().GetCurrentFrame();

		auto& target = frame.GetTarget();

		auto& attachment = target.GetColorAttachment(0);

		VkRenderingAttachmentInfo color{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
				.imageView = attachment.GetView().GetHandle(),
				.imageLayout = attachment.GetLayout(),
				.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		};

		commandBuffer.BeginRendering({
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = {
				.offset = { 0, 0 },
				.extent = target.GetExtent(),
			},
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &color,
		});

		commandBuffer.SetPipelineRenderingState({
			.colorAttachmentFormats = { attachment.GetFormat() }
		});

		Vulkan::VertexInputState vertexInputState{};
		vertexInputState.attributes.resize(3);
		vertexInputState.attributes[0].location = 0;
		vertexInputState.attributes[0].binding = 0;
		vertexInputState.attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputState.attributes[0].offset = offsetof(ImDrawVert, pos);

		vertexInputState.attributes[1].location = 1;
		vertexInputState.attributes[1].binding = 0;
		vertexInputState.attributes[1].format = VK_FORMAT_R32G32_SFLOAT;
		vertexInputState.attributes[1].offset = offsetof(ImDrawVert, uv);

		vertexInputState.attributes[2].location = 2;
		vertexInputState.attributes[2].binding = 0;
		vertexInputState.attributes[2].format = VK_FORMAT_R8G8B8A8_UNORM;
		vertexInputState.attributes[2].offset = offsetof(ImDrawVert, col);

		vertexInputState.bindings.resize(1);
		vertexInputState.bindings[0].binding = 0;
		vertexInputState.bindings[0].stride = sizeof(ImDrawVert);
		vertexInputState.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		commandBuffer.SetVertexInputState(vertexInputState);

		Vulkan::ColorBlendAttachmentState colorAttachment{};
		colorAttachment.blendEnable = VK_TRUE;

		Vulkan::ColorBlendState colorBlendState;
		colorBlendState.attachments.push_back(colorAttachment);

		commandBuffer.SetColorBlendState(colorBlendState);

		commandBuffer.SetRasterizationState({
			.cullMode = VK_CULL_MODE_NONE,
		});

		commandBuffer.SetDepthStencilState({
			.depthTestEnable = VK_FALSE,
			.depthWriteEnable = VK_FALSE,
		});

		commandBuffer.BindPipelineLayout(*pipelineLayout);

		commandBuffer.BindImage(fontTexture->GetImageView(), fontTexture->GetSampler(), 0, 0, 0);

		auto& io = ImGui::GetIO();

		auto transform = glm::translate(glm::mat4{ 1 }, glm::vec3{ -1.0f, -1.0f, 0.0f });
		transform = glm::scale(transform, glm::vec3{ 2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y, 0.0f });

		commandBuffer.PushConstants(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &transform);

		ImDrawData* drawData = ImGui::GetDrawData();

		size_t vertexBufferSize = drawData->TotalVtxCount * sizeof(ImDrawVert);
		size_t indexBufferSize = drawData->TotalIdxCount * sizeof(ImDrawIdx);

		std::vector<uint8_t> vertices(vertexBufferSize);
		std::vector<uint8_t> indexes(indexBufferSize);

		auto* verticesData = reinterpret_cast<ImDrawVert*>(vertices.data());
		auto* indexesData = reinterpret_cast<ImDrawIdx*>(indexes.data());

		for (int i = 0; i < drawData->CmdListsCount; i++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[i];

			memcpy(verticesData, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
			memcpy(indexesData, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
			verticesData += cmdList->VtxBuffer.Size;
			indexesData += cmdList->IdxBuffer.Size;
		}

		auto vertexAllocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::Vertex, vertexBufferSize);
		vertexAllocation.SetData(vertices.data());

		auto indexAllocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::Index, indexBufferSize);
		indexAllocation.SetData(indexes.data());

		VkDeviceSize vertexOffset[] = { vertexAllocation.GetOffset() };
		vkCmdBindVertexBuffers(commandBuffer.GetHandle(), 0, 1, &vertexAllocation.GetBuffer().GetHandle(), vertexOffset);

		vkCmdBindIndexBuffer(commandBuffer.GetHandle(), indexAllocation.GetBuffer().GetHandle(), indexAllocation.GetOffset(), VK_INDEX_TYPE_UINT16);

		uint32_t vertex = 0;
		uint32_t index = 0;
		for (int32_t i = 0; i < drawData->CmdListsCount; i++)
		{
			const ImDrawList* cmdList = drawData->CmdLists[i];
			for (int32_t j = 0; j < cmdList->CmdBuffer.Size; j++)
			{
				const ImDrawCmd* cmd = &cmdList->CmdBuffer[j];
				VkRect2D scissor{};
				scissor.offset.x = std::max(static_cast<int32_t>(cmd->ClipRect.x), 0);
				scissor.offset.y = std::max(static_cast<int32_t>(cmd->ClipRect.y), 0);
				scissor.extent.width = static_cast<uint32_t>(cmd->ClipRect.z - cmd->ClipRect.x);
				scissor.extent.height = static_cast<uint32_t>(cmd->ClipRect.w - cmd->ClipRect.y);

				commandBuffer.SetScissor({scissor});
				commandBuffer.DrawIndexed(cmd->ElemCount, 1, index, vertex, 0);
				index += cmd->ElemCount;
			}
			vertex += cmdList->VtxBuffer.Size;
		}

		commandBuffer.EndRendering();
	}

	bool Gui::OnInputEvent(const InputEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		return io.WantCaptureKeyboard || io.WantCaptureMouse;
	}
};