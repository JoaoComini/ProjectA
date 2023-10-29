#pragma once

#include "ShaderModule.hpp"

#include <spirv_glsl.hpp>

namespace Vulkan
{

	class SpirvReflection
	{
	public:
		SpirvReflection(VkShaderStageFlagBits stage, std::vector<uint32_t>& spirv);

		void ReflectShaderResources(std::vector<ShaderResource>& shaderResources);

	private:
		spirv_cross::CompilerGLSL compiler;
		VkShaderStageFlagBits stage;

		template<ShaderResourceType>
		void ParseShaderResource(std::vector<ShaderResource>& shaderResources);

		template<>
		void ParseShaderResource<ShaderResourceType::Input>(std::vector<ShaderResource>& shaderResources);

		template<>
		void ParseShaderResource<ShaderResourceType::Output>(std::vector<ShaderResource>& shaderResources);

		template<>
		void ParseShaderResource<ShaderResourceType::BufferUniform>(std::vector<ShaderResource>& shaderResources);

		template<>
		void ParseShaderResource<ShaderResourceType::ImageSampler>(std::vector<ShaderResource>& shaderResources);


		void ParseResourceVecSize(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		void ParseResourceArraySize(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		void ParseResourceSize(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		template<spv::Decoration>
		void ParseResourceDecoration(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		template<>
		void ParseResourceDecoration<spv::DecorationLocation>(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		template<>
		void ParseResourceDecoration<spv::DecorationDescriptorSet>(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		template<>
		void ParseResourceDecoration<spv::DecorationBinding>(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		template<ShaderResourceType T>
		ShaderResource CreateShaderResource(const spirv_cross::Resource& resource)
		{
			return {
				.type = T,
				.stages = static_cast<VkShaderStageFlags>(stage),
				.name = resource.name
			};
		}
	};
};