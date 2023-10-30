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
		PipelineLayout(const Device& device, const std::vector<ShaderModule*>& shaderModules);
		~PipelineLayout();

		DescriptorSetLayout& GetDescriptorSetLayout(uint32_t set);

		const std::vector<ShaderModule*>& GetShaderModules() const;

		bool HasShaderResource(ShaderResourceType type) const;

	private:
		void PrepareSetResources();
		void PrepareShaderSets();
		void CreateDescriptorSetLayouts();

		std::unordered_map<uint64_t, ShaderResource> setResources;
		std::map<uint32_t, std::vector<ShaderResource>> shaderSets;

		std::unordered_map<ShaderResourceType, std::vector<ShaderResource>> shaderResourceLookUp;

		std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;

		std::vector<ShaderModule*> shaderModules;

		const Device& device;
	};
}

namespace std
{
	template <>
	struct hash<Vulkan::PipelineLayout>
	{
		size_t operator()(const Vulkan::PipelineLayout& layout) const
		{
			std::size_t hash{ 0 };

			Hash(hash, layout.GetHandle());

			return hash;
		}
	};
};