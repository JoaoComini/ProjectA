#include "PipelineLayout.hpp"

#include "Device.hpp"

namespace Vulkan
{

	PipelineLayout::PipelineLayout(const Device& device, const std::vector<Engine::Shader*>& shaders)
		: device(device), shaders(shaders)
	{
		PrepareSetResources();
		PrepareShaderSets();
		CreateDescriptorSetLayouts();

		std::vector<VkDescriptorSetLayout> handles(descriptorSetLayouts.size());
		std::transform(descriptorSetLayouts.begin(), descriptorSetLayouts.end(), handles.begin(), [](auto& layout) { return layout->GetHandle(); });

		std::vector<VkPushConstantRange> pushConstantRanges;
		auto it = shaderResourceLookUp.find(Engine::ShaderResourceType::PushConstant);
		if (it != shaderResourceLookUp.end())
		{

			for (auto& shaderResource : it->second)
			{
				VkShaderStageFlags stageFlags{};

				if (bool(shaderResource.stages & Engine::ShaderStage::Vertex))
				{
					stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
				}

				if (bool(shaderResource.stages & Engine::ShaderStage::Fragment))
				{
					stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
				}

				pushConstantRanges.push_back({ stageFlags, 0, shaderResource.size });
			}
		}

		VkPipelineLayoutCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.setLayoutCount = handles.size();
		createInfo.pSetLayouts = handles.data();
		createInfo.pushConstantRangeCount = pushConstantRanges.size();
		createInfo.pPushConstantRanges = pushConstantRanges.data();

		if (vkCreatePipelineLayout(device.GetHandle(), &createInfo, nullptr, &handle) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void PipelineLayout::PrepareSetResources()
	{
		for (auto& shader : shaders)
		{
			for (auto& shaderResource : shader->GetResources())
			{
				// Ignore inputs, outputs and push constants since they don't belong to a descriptor set
				if (shaderResource.type == Engine::ShaderResourceType::Input
					|| shaderResource.type == Engine::ShaderResourceType::Output
					|| shaderResource.type == Engine::ShaderResourceType::PushConstant)
				{
					shaderResourceLookUp[shaderResource.type].push_back(shaderResource);

					continue;
				}

				// Hashes a unique set + binding key
				uint64_t key = static_cast<uint64_t>(shaderResource.set) << 32 | shaderResource.binding;

				auto it = setResources.find(key);
				
				// If the resource exists on other shader modules, it must be on 
				// multiple shader stages, hence merge then.
				if (it != setResources.end())
				{
					it->second.stages |= shader->GetStage();
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

	DescriptorSetLayout& PipelineLayout::GetDescriptorSetLayout(uint32_t set) const
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

	const std::vector<Engine::Shader*>& PipelineLayout::GetShaders() const
	{
		return shaders;
	}

	bool PipelineLayout::HasShaderResource(Engine::ShaderResourceType type) const
	{
		return shaderResourceLookUp.find(type) != shaderResourceLookUp.end();
	}

	PipelineLayout::~PipelineLayout()
	{
		vkDestroyPipelineLayout(device.GetHandle(), handle, nullptr);
	}
}