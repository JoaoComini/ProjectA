#include "Pipeline.hpp"

#include "ShaderModule.hpp"

namespace Vulkan
{
	Pipeline::Pipeline(const Device& device, PipelineState state) : device(device)
	{
		auto pipelineLayout = state.GetPipelineLayout();

		std::vector<VkShaderModule> shaderModules;
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
	
		for (auto& shaderModule : pipelineLayout->GetShaderModules())
		{
			VkPipelineShaderStageCreateInfo stageCreateinfo{};
			stageCreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			stageCreateinfo.stage = shaderModule->GetStage();
			stageCreateinfo.pName = "main";

			auto& spirv = shaderModule->GetSpirv();

			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = spirv.size() * sizeof(uint32_t);
			createInfo.pCode = spirv.data();

			if (vkCreateShaderModule(device.GetHandle(), &createInfo, nullptr, &stageCreateinfo.module) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create shader module!");
			}

			shaderStages.push_back(stageCreateinfo);
			shaderModules.push_back(stageCreateinfo.module);
		}

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		auto& vertexInputState = state.GetVertexInputState();
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = vertexInputState.bindings.size();
		vertexInputInfo.pVertexBindingDescriptions = vertexInputState.bindings.data();
		vertexInputInfo.vertexAttributeDescriptionCount = vertexInputState.attributes.size();
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputState.attributes.data();

		auto& inputAssemblyState = state.GetInputAssemblyState();
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = inputAssemblyState.topology;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		auto& rasterizationState = state.GetRasterizationState();
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = rasterizationState.cullMode;
		rasterizer.frontFace = rasterizationState.frontFace;

		auto& multisampleState = state.GetMultisampleState();
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.rasterizationSamples = multisampleState.rasterizationSamples;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 1.0f;
		colorBlending.blendConstants[1] = 1.0f;
		colorBlending.blendConstants[2] = 1.0f;
		colorBlending.blendConstants[3] = 1.0f;

		auto& depthStencilState = state.GetDepthStencilState();
		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = depthStencilState.depthTestEnable;
		depthStencil.depthWriteEnable = depthStencilState.depthWriteEnable;
		depthStencil.depthCompareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.pNext = nullptr;

		auto& pipelineRenderingState = state.GetPipelineRenderingState();
		VkPipelineRenderingCreateInfo pipelineRendering{};
		pipelineRendering.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRendering.colorAttachmentCount = pipelineRenderingState.colorAttachmentFormats.size();
		pipelineRendering.pColorAttachmentFormats = pipelineRenderingState.colorAttachmentFormats.data();
		pipelineRendering.depthAttachmentFormat = pipelineRenderingState.depthAttachmentFormat;
		pipelineRendering.pNext = nullptr;

		VkGraphicsPipelineCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.stageCount = shaderStages.size();
		createInfo.pStages = shaderStages.data();
		createInfo.pVertexInputState = &vertexInputInfo;
		createInfo.pInputAssemblyState = &inputAssembly;
		createInfo.pViewportState = &viewportState;
		createInfo.pRasterizationState = &rasterizer;
		createInfo.pMultisampleState = &multisampling;
		createInfo.pDepthStencilState = &depthStencil;
		createInfo.pColorBlendState = &colorBlending;
		createInfo.pDynamicState = &dynamicState;
		createInfo.layout = pipelineLayout->GetHandle();
		createInfo.basePipelineHandle = VK_NULL_HANDLE;
		createInfo.basePipelineIndex = -1;
		createInfo.pNext = &pipelineRendering;

		if (vkCreateGraphicsPipelines(device.GetHandle(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		for (auto shaderModule : shaderModules)
		{
			vkDestroyShaderModule(device.GetHandle(), shaderModule, nullptr);
		}
	}

	Pipeline::~Pipeline()
	{
		vkDestroyPipeline(device.GetHandle(), handle, nullptr);
	}
};