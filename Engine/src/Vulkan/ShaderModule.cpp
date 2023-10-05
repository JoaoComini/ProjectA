#include "ShaderModule.hpp"

#include <iostream>

#include "Common/FileSystem.hpp"

namespace Vulkan
{
    ShaderModule::ShaderModule(const Device &device, std::string path) : device(device)
    {
        auto content = FileSystem::ReadFile(path);

        std::vector<uint8_t> code(content.begin(), content.end());

        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = code.size();
        info.pCode = reinterpret_cast<uint32_t *>(code.data());

        if (vkCreateShaderModule(device.GetHandle(), &info, nullptr, &handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
    }

    ShaderModule::~ShaderModule()
    {
        vkDestroyShaderModule(device.GetHandle(), handle, nullptr);
    }
}
