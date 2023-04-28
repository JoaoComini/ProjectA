#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <vulkan/vulkan.h>

class VulkanShader
{    
public:
    VulkanShader(VkDevice device, std::string path);
    ~VulkanShader();
    VkShaderModule GetModule();

private:
    VkShaderModule module;
    VkDevice device;

private:
    std::vector<unsigned char> ReadFile(const std::string &path);
};

