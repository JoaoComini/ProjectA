#pragma once
#include "Shader.hpp"

#include "SpirvReflection.hpp"
#include "GlslCompiler.hpp"

#include "Common/Hash.hpp"

namespace Engine
{
    ShaderModule::ShaderModule(ShaderStage stage, const ShaderSource& source, const ShaderVariant& variant)
        : stage(stage)
    {
        GlslCompiler glslCompiler{};
        glslCompiler.CompileToSpv(stage, source, variant, spirv);

        SpirvReflection spirvReflection{ stage, spirv };
        spirvReflection.ReflectShaderResources(shaderResources);

        Hash(hash, stage, source.GetHash(), variant.GetHash());
    }

    ShaderStage ShaderModule::GetStage() const
    {
        return stage;
    }

    const std::vector<ShaderResource>& ShaderModule::GetResources() const
    {
        return shaderResources;
    }

    const std::vector<uint32_t>& ShaderModule::GetSpirv() const
    {
        return spirv;
    }

    size_t ShaderModule::GetHash() const
    {
        return hash;
    }

    ShaderSource::ShaderSource(const std::vector<uint8_t>& bytes)
    {
        source = { bytes.begin(), bytes.end() };

        Hash(hash, source);
    }

    const std::string& ShaderSource::GetSource() const
    {
        return source;
    }

    size_t ShaderSource::GetHash() const
    {
        return hash;
    }


    void ShaderVariant::AddDefine(const std::string& define)
    {
        preamble.append("#define " + define + "\n");

        hash = 0;
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
