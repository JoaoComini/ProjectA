#pragma once

#include <vulkan/vulkan.h>

#include <vector>

#include "Resource.hpp"
#include "DescriptorSetLayout.hpp"
#include "ShaderModule.hpp"

#include <unordered_map>
#include <map>

namespace Vulkan
{
	class Device;

	class PipelineLayout : public Resource<VkPipelineLayout>
	{
	public:
		PipelineLayout(const Device& device, const std::vector<std::shared_ptr<ShaderModule>>& shaderModules);
		~PipelineLayout();

		DescriptorSetLayout& GetDescriptorSetLayout(uint32_t set);

	private:
		void PrepareSetResources(const std::vector<std::shared_ptr<ShaderModule>>& shaderModules);
		void PrepareShaderSets();
		void CreateDescriptorSetLayouts();

		std::unordered_map<uint64_t, ShaderResource> setResources;
		std::map<uint32_t, std::vector<ShaderResource>> shaderSets;

		std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;

		const Device& device;
	};
}