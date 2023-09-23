#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <vulkan/vulkan.h>

#include "Resource.hpp"
#include "Device.hpp"

namespace Vulkan
{

    class Shader : public Resource<VkShaderModule>
    {
    public:
        Shader(const Device &device, std::string path);
        ~Shader();

    private:
        const Device &device;

    private:
        std::vector<unsigned char> ReadFile(const std::string &path);
    };

}
