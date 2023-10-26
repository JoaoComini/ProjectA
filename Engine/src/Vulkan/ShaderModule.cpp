#include "ShaderModule.hpp"

#include <iostream>
#include <spirv_glsl.hpp>

#include "SpirvReflection.hpp"

namespace Vulkan
{
    ShaderSource::ShaderSource(const std::filesystem::path& path)
    {
        source = FileSystem::ReadFile(path);
    }

    const std::string& ShaderSource::GetSource() const
    {
        return source;
    }

    ShaderModule::ShaderModule(ShaderStage stage, const ShaderSource& source)
        : stage(stage)
    {
        auto& src = source.GetSource();

        spirv.resize(src.size() / sizeof(uint32_t));
        std::memcpy(spirv.data(), src.data(), src.size());

        SpirvReflection spirvReflection{ spirv, stage };
        spirvReflection.ReflectShaderResources(shaderResources);
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

}
