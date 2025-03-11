#include "DescriptorSetLayout.hpp"

#include "Device.hpp"

namespace Vulkan
{
	VkDescriptorType DescriptorTypeFromShaderResourceType(Engine::ShaderResourceType type)
	{
		switch (type)
		{
		case Engine::ShaderResourceType::ImageSampler:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		case Engine::ShaderResourceType::BufferUniform:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		default:
			throw std::runtime_error("No conversion possible for the shader resource type.");
		}
	}

	DescriptorSetLayout::DescriptorSetLayout(const Device& device, const uint32_t set, const std::vector<Engine::ShaderResource>& setResources)
		: device(device), set(set)
	{
		for (auto& shaderResource : setResources)
		{
			VkShaderStageFlags stageFlags{};
			
			if (static_cast<bool>(shaderResource.stages & Engine::ShaderStage::Vertex))
			{
				stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
			}

			if (static_cast<bool>(shaderResource.stages & Engine::ShaderStage::Fragment))
			{
				stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			}

			VkDescriptorSetLayoutBinding binding{
				.binding = shaderResource.binding,
				.descriptorType = DescriptorTypeFromShaderResourceType(shaderResource.type),
				.descriptorCount = shaderResource.arraySize,
				.stageFlags = stageFlags
			};

			bindings.push_back(binding);
		}

		const VkDescriptorSetLayoutCreateInfo createInfo = {
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

	const VkDescriptorSetLayoutBinding* DescriptorSetLayout::GetBinding(const uint32_t binding) const
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