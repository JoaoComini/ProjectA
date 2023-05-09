#include "Shader.hpp"

#include <iostream>

namespace Vulkan
{
    Shader::Shader(const Device &device, std::string path) : device(device)
    {
        auto code = ReadFile(path);

        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = code.size();
        info.pCode = reinterpret_cast<const uint32_t *>(code.data());

        if (vkCreateShaderModule(device.GetHandle(), &info, nullptr, &handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }
    }

    std::vector<unsigned char> Shader::ReadFile(const std::string &path)
    {
        std::ifstream file(path, std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file!");
        }

        return std::vector<unsigned char>(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    }


    Shader::~Shader()
    {
        vkDestroyShaderModule(device.GetHandle(), handle, nullptr);
    }
}
