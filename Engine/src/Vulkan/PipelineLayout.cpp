#include "PipelineLayout.hpp"

#include "Device.hpp"

namespace Vulkan
{

	PipelineLayout::PipelineLayout(const Device& device, const std::vector<std::shared_ptr<ShaderModule>>& shaderModules)
		: device(device)
	{
		PrepareSetResources(shaderModules);
		PrepareShaderSets();
		CreateDescriptorSetLayouts();

		std::vector<VkDescriptorSetLayout> handles(descriptorSetLayouts.size());
		std::transform(descriptorSetLayouts.begin(), descriptorSetLayouts.end(), handles.begin(), [](auto& layout) { return layout->GetHandle(); });

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.setLayoutCount = handles.size();
		createInfo.pSetLayouts = handles.data();
		createInfo.pushConstantRangeCount = 0;
		createInfo.pPushConstantRanges = VK_NULL_HANDLE;

		if (vkCreatePipelineLayout(device.GetHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void PipelineLayout::PrepareSetResources(const std::vector<std::shared_ptr<ShaderModule>>& shaderModules)
	{
		for (auto& shaderModule : shaderModules)
		{
			for (auto& shaderResource : shaderModule->GetResources())
			{
				// Ignore inputs and outputs since they don't belong to a descriptor set
				if (shaderResource.type == ShaderResourceType::Input || shaderResource.type == ShaderResourceType::Output)
				{
					continue;
				}

				// Hashes a unique set + binding key
				uint64_t key = static_cast<uint64_t>(shaderResource.set) << 32 | shaderResource.binding;

				auto it = setResources.find(key);
				
				// If the resource exists on both shader modules, so it must be on 
				// both shader stages, hence merge then.
				if (it != setResources.end())
				{
					// We may need to change this when adding support for 
					// shader stages other than Vextex and Fragment
					it->second.stage = ShaderStage::Both;
				}
				else
				{
					setResources[key] = shaderResource;
				}
			}
		}
	}

	/* 
		Creates a map of descriptor set index to resources.
		Each entry represents a DescriptorSet that will be
		created on CreateDescriptorSetLayouts().
	*/
	void PipelineLayout::PrepareShaderSets()
	{
		for (auto& [_, shaderResource] : setResources)
		{
			auto it = shaderSets.find(shaderResource.set);

			if (it != shaderSets.end())
			{
				it->second.push_back(shaderResource);
			}
			else
			{
				shaderSets[shaderResource.set].push_back(shaderResource);
			}
		}
	}

	void PipelineLayout::CreateDescriptorSetLayouts()
	{
		for (const auto& [set, shaderResources] : shaderSets)
		{
			descriptorSetLayouts.push_back(std::make_shared<Vulkan::DescriptorSetLayout>(
				device,
				set,
				shaderResources
			));
		}
	}

	DescriptorSetLayout& PipelineLayout::GetDescriptorSetLayout(uint32_t set)
	{
		for (auto& layout : descriptorSetLayouts)
		{
			if (layout->GetSet() == set)
			{
				return *layout;
			}
		}

		throw std::runtime_error("couldn't find descriptor set layout at set " + std::to_string(set));
	}

	PipelineLayout::~PipelineLayout()
	{
		vkDestroyPipelineLayout(device.GetHandle(), handle, nullptr);
	}
}