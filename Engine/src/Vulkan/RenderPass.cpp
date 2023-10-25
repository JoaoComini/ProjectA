#include "RenderPass.hpp"

namespace Vulkan
{
	RenderPass::RenderPass(const Device& device, const std::vector<AttachmentInfo>& attachments, const std::vector<LoadStoreInfo>& loadStoreInfos, const std::vector<SubpassInfo>& subpasses)
		: device(device)
	{
		std::vector<VkAttachmentDescription> attachmentDescriptions;

		for (size_t i = 0; i < attachments.size(); i++)
		{
			VkAttachmentDescription description{};

			description.format = attachments[i].format;
			description.samples = attachments[i].samples;
			description.initialLayout = attachments[i].initialLayout;
			description.finalLayout = description.format == VK_FORMAT_D32_SFLOAT ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			if (i < loadStoreInfos.size())
			{
				description.loadOp = loadStoreInfos[i].loadOp;
				description.storeOp = loadStoreInfos[i].storeOp;
				description.stencilLoadOp = loadStoreInfos[i].loadOp;
				description.stencilStoreOp = loadStoreInfos[i].storeOp;
			}

			attachmentDescriptions.push_back(std::move(description));
		}

		std::vector<std::vector<VkAttachmentReference>> inputReferences{ subpasses.size() };
		std::vector<std::vector<VkAttachmentReference>> colorReferences{ subpasses.size() };
		std::vector<std::vector<VkAttachmentReference>> depthStencilReferences{ subpasses.size() };
		std::vector<std::vector<VkAttachmentReference>> colorResolveReferences{ subpasses.size() };

		for (size_t i = 0; i < subpasses.size(); i++)
		{
			auto& subpass = subpasses[i];

			for (auto input : subpass.inputAttachments)
			{
				auto layout = attachmentDescriptions[input].format == VK_FORMAT_D32_SFLOAT
					? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
					: VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				if (attachments[input].initialLayout != VK_IMAGE_LAYOUT_UNDEFINED)
				{
					layout = attachments[input].initialLayout;
				}

				VkAttachmentReference reference{};
				reference.attachment = input;
				reference.layout = layout;

				inputReferences[i].push_back(std::move(reference));
			}

			for (auto output : subpass.outputAttachments)
			{
				auto& description = attachmentDescriptions[output];

				if (description.format == VK_FORMAT_D32_SFLOAT)
				{
					continue;
				}

				auto layout = attachments[output].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
					? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
					: attachments[output].initialLayout;

				VkAttachmentReference reference{};
				reference.attachment = output;
				reference.layout = layout;

				colorReferences[i].push_back(std::move(reference));
			}

			for (auto resolve : subpass.colorResolveAttachments)
			{
				auto layout = attachments[resolve].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
					? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL 
					: attachments[resolve].initialLayout;

				VkAttachmentReference reference{};
				reference.attachment = resolve;
				reference.layout = layout;

				colorResolveReferences[i].push_back(std::move(reference));
			}

			if (!subpass.disableDepthStencilAttachment)
			{
				auto it = std::find_if(attachments.begin(), attachments.end(), [](const AttachmentInfo info) { return info.format == VK_FORMAT_D32_SFLOAT; });

				if (it != attachments.end())
				{
					size_t depthStencil = std::distance(attachments.begin(), it);

					auto layout = it->initialLayout == VK_IMAGE_LAYOUT_UNDEFINED
						? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL 
						: it->initialLayout;

					VkAttachmentReference reference{};
					reference.attachment = depthStencil;
					reference.layout = layout;

					depthStencilReferences[i].push_back(std::move(reference));
				}
			}
		}

		std::vector<VkSubpassDescription> subpassDescriptions;
		for (size_t i = 0; i < subpasses.size(); i++)
		{
			auto& subpass = subpasses[i];
			
			VkSubpassDescription description{};
			description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			description.pInputAttachments = inputReferences[i].empty() ? nullptr : inputReferences[i].data();
			description.inputAttachmentCount = inputReferences[i].size();

			description.pColorAttachments = colorReferences[i].empty() ? nullptr : colorReferences[i].data();
			description.colorAttachmentCount = colorReferences[i].size();

			description.pResolveAttachments = colorResolveReferences[i].empty() ? nullptr : colorResolveReferences[i].data();

			if (!depthStencilReferences[i].empty())
			{
				description.pDepthStencilAttachment = depthStencilReferences[i].data();
			}

			subpassDescriptions.push_back(std::move(description));
		}

		for (auto& subpass : subpassDescriptions)
		{
			for (size_t i = 0; i < subpass.colorAttachmentCount; i++)
			{
				auto& reference = subpass.pColorAttachments[i];
				// Set it only if not defined yet
				if (attachmentDescriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
				{
					attachmentDescriptions[reference.attachment].initialLayout = reference.layout;
				}
			}

			for (size_t i = 0; i < subpass.inputAttachmentCount; i++)
			{
				auto& reference = subpass.pInputAttachments[i];
				// Set it only if not defined yet
				if (attachmentDescriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
				{
					attachmentDescriptions[reference.attachment].initialLayout = reference.layout;
				}
			}

			if (subpass.pDepthStencilAttachment)
			{
				auto& reference = *subpass.pDepthStencilAttachment;
				// Set it only if not defined yet
				if (attachmentDescriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
				{
					attachmentDescriptions[reference.attachment].initialLayout = reference.layout;
				}
			}

			if (subpass.pResolveAttachments)
			{
				for (size_t i = 0; i < subpass.colorAttachmentCount; i++)
				{
					auto& reference = subpass.pResolveAttachments[i];
					// Set it only if not defined yet
					if (attachmentDescriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
					{
						attachmentDescriptions[reference.attachment].initialLayout = reference.layout;
					}
				}
			}
		}

		std::vector<VkSubpassDependency> dependencies(subpasses.size() - 1);
		for (size_t i = 0; i < dependencies.size(); i++)
		{
			dependencies[i].srcSubpass = i;
			dependencies[i].dstSubpass = i + 1;
			dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			dependencies[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
			dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = attachmentDescriptions.size();
		createInfo.pAttachments = attachmentDescriptions.data();
		createInfo.subpassCount = subpassDescriptions.size();
		createInfo.pSubpasses = subpassDescriptions.data();
		createInfo.dependencyCount = dependencies.size();
		createInfo.pDependencies = dependencies.data();

		if (vkCreateRenderPass(device.GetHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!");
		}
	}

	RenderPass::~RenderPass()
	{
		vkDestroyRenderPass(device.GetHandle(), handle, nullptr);
	}
};