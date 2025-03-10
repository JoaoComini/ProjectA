#pragma once

#include "Resource.hpp"
#include "DescriptorSetLayout.hpp"

namespace Vulkan
{
	class Device;

	class PipelineLayout : public Resource<VkPipelineLayout>
	{
	public:
		PipelineLayout(const Device& device, const std::vector<Engine::Shader*>& shader);
		~PipelineLayout();

		DescriptorSetLayout& GetDescriptorSetLayout(uint32_t set) const;

		const std::vector<Engine::Shader*>& GetShaders() const;

		bool HasShaderResource(Engine::ShaderResourceType type) const;

	private:
		void PrepareSetResources();
		void PrepareShaderSets();
		void CreateDescriptorSetLayouts();

		std::unordered_map<uint64_t, Engine::ShaderResource> setResources;
		std::map<uint32_t, std::vector<Engine::ShaderResource>> shaderSets;

		std::unordered_map<Engine::ShaderResourceType, std::vector<Engine::ShaderResource>> shaderResourceLookUp;

		std::vector<std::shared_ptr<DescriptorSetLayout>> descriptorSetLayouts;

		std::vector<Engine::Shader*> shaders;

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