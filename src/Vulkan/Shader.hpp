#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <vulkan/vulkan.h>

namespace Vulkan
{

    class Shader
    {
    public:
        Shader(VkDevice device, std::string path);
        ~Shader();
        VkShaderModule GetModule();

    private:
        VkShaderModule module;
        VkDevice device;

    private:
        std::vector<unsigned char> ReadFile(const std::string &path);
    };

}
