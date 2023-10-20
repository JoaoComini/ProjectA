#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "Resource.hpp"
#include "Device.hpp"

#include "Common/FileSystem.hpp"

namespace Vulkan
{

    enum class ShaderStage
    {
        Vertex = VK_SHADER_STAGE_VERTEX_BIT,
        Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
        Both = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
    };

    class ShaderSource
    {
    public:
        ShaderSource(const std::filesystem::path& path);

        const std::string& GetSource() const;
    private:
        std::string source;
    };

    enum class ShaderResourceType
    {
        Input,
        Output,
        ImageSampler,
        BufferUniform,
        PushConstant,
        All
    };

    struct ShaderResource
    {
        ShaderResourceType type;
        ShaderStage stage;
        std::string name;

        uint32_t set;
        uint32_t binding;
        uint32_t location;
        uint32_t vecSize;
        uint32_t columns;
        uint32_t arraySize;
        uint32_t size;
    };

    class ShaderModule : public Resource<VkShaderModule>
    {
    public:
        ShaderModule(const Device& device, ShaderStage stage, const ShaderSource& source);

        ~ShaderModule();

        ShaderStage GetStage();

        const std::vector<ShaderResource>& GetResources();

    private:
        ShaderStage stage{};
        std::vector<ShaderResource> shaderResources;

        const Device &device;
    };

}
