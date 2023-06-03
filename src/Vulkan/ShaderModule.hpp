#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "Resource.hpp"
#include "Device.hpp"

namespace Vulkan
{

    class ShaderModule : public Resource<VkShaderModule>
    {
    public:
        ShaderModule(const Device &device, std::string path);
        ~ShaderModule();

    private:
        const Device &device;
    };

}
