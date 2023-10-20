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

    ShaderModule::ShaderModule(const Device& device, ShaderStage stage, const ShaderSource& source)
        : device(device), stage(stage)
    {
        auto& src = source.GetSource();

        std::vector<uint32_t> spirv;
        spirv.resize(src.size() / sizeof(uint32_t));

        std::memcpy(spirv.data(), src.data(), src.size());

        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = spirv.size() * sizeof(uint32_t);
        info.pCode = spirv.data();

        if (vkCreateShaderModule(device.GetHandle(), &info, nullptr, &handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }

        SpirvReflection spirvReflection{ spirv, stage };
        spirvReflection.ReflectShaderResources(shaderResources);
    }

    ShaderModule::~ShaderModule()
    {
        vkDestroyShaderModule(device.GetHandle(), handle, nullptr);
    }

    ShaderStage ShaderModule::GetStage()
    {
        return stage;
    }

    const std::vector<ShaderResource>& ShaderModule::GetResources()
    {
        return shaderResources;
    }

}
