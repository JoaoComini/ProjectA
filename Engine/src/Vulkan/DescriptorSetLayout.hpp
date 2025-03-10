#pragma once

#include "Common/Hash.hpp"

#include "Resource.hpp"
#include "Rendering/Shader.hpp"

namespace Vulkan
{
	class Device;

	class DescriptorSetLayout : public Resource<VkDescriptorSetLayout>
	{
	public:
		DescriptorSetLayout(const Device& device, uint32_t set, const std::vector<Engine::ShaderResource>& setResources);
		~DescriptorSetLayout();

		const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const;

		const VkDescriptorSetLayoutBinding* GetBinding(uint32_t binding) const;

		uint32_t GetSet() const;

	private:
		const Device& device;

		uint32_t set;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};
};

namespace std
{
	template <>
	struct hash<Vulkan::DescriptorSetLayout>
	{
		size_t operator()(const Vulkan::DescriptorSetLayout& layout) const
		{
			std::size_t hash{ 0 };

			HashCombine(hash, layout.GetHandle());

			return hash;
		}
	};
};