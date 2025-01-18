#pragma once
#include "Shader.hpp"

#include "SpirvReflection.hpp"
#include "GlslCompiler.hpp"

#include "Common/Hash.hpp"

namespace Engine
{
    Shader::Shader(ShaderStage stage, const ShaderSource& source, const ShaderVariant& variant)
        : stage(stage)
    {
        GlslCompiler glslCompiler{};
        glslCompiler.CompileToSpv(stage, source, variant, spirv);

        SpirvReflection spirvReflection{ stage, spirv };
        spirvReflection.ReflectShaderResources(shaderResources);

        Hash(hash, std::string{ reinterpret_cast<const char*>(spirv.data()), reinterpret_cast<const char*>(spirv.data() + spirv.size()) });
    }

    ShaderStage Shader::GetStage() const
    {
        return stage;
    }

    const std::vector<ShaderResource>& Shader::GetResources() const
    {
        return shaderResources;
    }

    const std::vector<uint32_t>& Shader::GetSpirv() const
    {
        return spirv;
    }

    size_t Shader::GetHash() const
    {
        return hash;
    }

    ShaderSource::ShaderSource(const std::vector<uint8_t>& bytes)
    {
        source = { bytes.begin(), bytes.end() };

        Hash(hash, reinterpret_cast<const char*>(source.data()), reinterpret_cast<const char*>(source.data() + source.size()));
    }

    const std::string& ShaderSource::GetSource() const
    {
        return source;
    }

    size_t ShaderSource::GetHash() const
    {
        return hash;
    }

    void ShaderVariant::AddDefinitions(const std::vector<std::string>& definitions)
    {
        for (const auto& define : definitions)
        {
            AddDefine(define);
        }
    }

    void ShaderVariant::AddDefine(const std::string& define)
    {
        preamble.append("#define " + define + "\n");

        Hash(hash, preamble);
    }

    const std::string& ShaderVariant::GetPreamble() const
    {
        return preamble;
    }

    size_t ShaderVariant::GetHash() const
    {
        return hash;
    }
}
