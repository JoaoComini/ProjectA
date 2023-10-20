#include "DescriptorSetLayout.hpp"

namespace Vulkan
{
	VkDescriptorType DescriptorTypeFromShaderResourceType(ShaderResourceType type)
	{
		switch (type)
		{
		case ShaderResourceType::ImageSampler:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			break;
		case ShaderResourceType::BufferUniform:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			break;
		default:
			throw std::runtime_error("No conversion possible for the shader resource type.");
			break;
		}
	}

	DescriptorSetLayout::DescriptorSetLayout(const Device& device, uint32_t set, const std::vector<ShaderResource>& setResources)
		: device(device), set(set)
	{
		for (auto& shaderResource : setResources)
		{
			VkDescriptorSetLayoutBinding binding = {
				.binding = shaderResource.binding,
				.descriptorType = DescriptorTypeFromShaderResourceType(shaderResource.type),
				.descriptorCount = shaderResource.arraySize,
				.stageFlags = static_cast<VkShaderStageFlags>(shaderResource.stage)
			};

			bindings.push_back(binding);
		}

		VkDescriptorSetLayoutCreateInfo createInfo = {
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.bindingCount = static_cast<uint32_t>(bindings.size()),
			.pBindings = bindings.data()
		};

		vkCreateDescriptorSetLayout(device.GetHandle(), &createInfo, nullptr, &handle);
	}

	DescriptorSetLayout::~DescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(device.GetHandle(), handle, nullptr);
	}

	const std::vector<VkDescriptorSetLayoutBinding>& DescriptorSetLayout::GetBindings() const
	{
		return bindings;
	}

	const VkDescriptorSetLayoutBinding* DescriptorSetLayout::GetBinding(uint32_t binding) const
	{
		for (auto& it : bindings)
		{
			if (it.binding == binding)
			{
				return &it;
			}
		}

		return nullptr;
	}

	uint32_t DescriptorSetLayout::GetSet() const
	{
		return set;
	}
};