#pragma once

#include "Shader.hpp"

#include <spirv_glsl.hpp>

namespace Engine
{

	class SpirvReflection
	{
	public:
		SpirvReflection(ShaderStage stage, std::vector<uint32_t>& spirv);

		void ReflectShaderResources(std::vector<ShaderResource>& shaderResources);

	private:
		spirv_cross::CompilerGLSL compiler;
		ShaderStage stage;

		template<ShaderResourceType>
		void ParseShaderResource(std::vector<ShaderResource>& shaderResources);

		void ParseResourceVecSize(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		void ParseResourceArraySize(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		void ParseResourceSize(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		template<spv::Decoration>
		void ParseResourceDecoration(const spirv_cross::Resource& resource, ShaderResource& shaderResource);

		template<ShaderResourceType T>
		ShaderResource CreateShaderResource(const spirv_cross::Resource& resource)
		{
			return {
				.type = T,
				.stages = stage,
				.name = resource.name
			};
		}
	};
};