#pragma once

#include "Resource.hpp"
#include "DescriptorSetLayout.hpp"

namespace Vulkan
{
	class Device;

	class PipelineLayout : public Resource<VkPipelineLayout>
	{
	public:
		PipelineLayout(const Device& device, const std::vector<Engine::ShaderModule*>& shader);
		~PipelineLayout() override;

		[[nodiscard]] DescriptorSetLayout& GetDescriptorSetLayout(uint32_t set) const;

		[[nodiscard]] const std::vector<Engine::ShaderModule*>& GetShaders() const;

		[[nodiscard]] bool HasShaderResource(Engine::ShaderResourceType type) const;

	private:
		void PrepareSetResources();
		void PrepareShaderSets();
		void CreateDescriptorSetLayouts();

		std::unordered_map<uint64_t, Engine::ShaderResource> setResources;
		std::map<uint32_t, std::vector<Engine::ShaderResource>> shaderSets;

		std::unordered_map<Engine::ShaderResourceType, std::vector<Engine::ShaderResource>> shaderResourceLookUp;

		std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;

		std::vector<Engine::ShaderModule*> shaders;

		const Device& device;
	};
}

template <>
struct std::hash<Vulkan::PipelineLayout>
{
	size_t operator()(const Vulkan::PipelineLayout& layout) const noexcept
	{
		std::size_t hash{ 0 };

		Hash(hash, layout.GetHandle());

		return hash;
	}
};;