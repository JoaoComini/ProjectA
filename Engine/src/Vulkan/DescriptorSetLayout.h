#pragma once

#include "Common/Hash.h"

#include "Resource.h"
#include "Rendering/Shader.h"

namespace Vulkan
{
	class Device;

	class DescriptorSetLayout : public Resource<VkDescriptorSetLayout>
	{
	public:
		DescriptorSetLayout(const Device& device, uint32_t set, const std::vector<Engine::ShaderResource>& setResources);
		~DescriptorSetLayout() override;

		[[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const;

		[[nodiscard]] const VkDescriptorSetLayoutBinding* GetBinding(uint32_t binding) const;

		[[nodiscard]] uint32_t GetSet() const;

	private:
		const Device& device;

		uint32_t set;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};
};

template <>
struct std::hash<Vulkan::DescriptorSetLayout>
{
	size_t operator()(const Vulkan::DescriptorSetLayout& layout) const noexcept
	{
		std::size_t hash{ 0 };

		HashCombine(hash, layout.GetHandle());

		return hash;
	}
};;