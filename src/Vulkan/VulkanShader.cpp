#include "VulkanShader.hpp"

#include <iostream>

VulkanShader::VulkanShader(VkDevice device, std::string path) : device(device)
{
    auto code = ReadFile(path);

    VkShaderModuleCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    info.codeSize = code.size();
    info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    if (vkCreateShaderModule(device, &info, nullptr, &module) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create shader module!");
    }
}

std::vector<unsigned char> VulkanShader::ReadFile(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

VkShaderModule VulkanShader::GetModule()
{
    return module;
}

VulkanShader::~VulkanShader()
{
    vkDestroyShaderModule(device, module, nullptr);
}
