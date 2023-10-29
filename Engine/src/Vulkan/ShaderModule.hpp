#pragma once

#include <string>
#include <vector>
#include <vulkan/vulkan.h>

#include "Resource.hpp"

#include "Common/FileSystem.hpp"
#include "Common/Hash.hpp"

namespace Vulkan
{
    class Device;

    class ShaderSource
    {
    public:
        ShaderSource(const std::filesystem::path& path);

        const std::string& GetSource() const;

        size_t GetHash() const;
    private:
        std::string source;

        size_t hash{ 0 };
    };

    enum class ShaderResourceType
    {
        Input,
        Output,
        ImageSampler,
        BufferUniform,
        PushConstant
    };

    struct ShaderResource
    {
        ShaderResourceType type;
        VkShaderStageFlags stages;
        std::string name;

        uint32_t set;
        uint32_t binding;
        uint32_t location;
        uint32_t vecSize;
        uint32_t columns;
        uint32_t arraySize;
        uint32_t size;
    };

    class ShaderVariant
    {
    public:
        void AddDefinitions(const std::vector<std::string>& definitions);
        void AddDefine(const std::string& define);
        const std::string& GetPreamble() const;

        size_t GetHash() const;

    private:
        std::string preamble;

        size_t hash{ 0 };
    };


    class ShaderModule
    {
    public:
        ShaderModule(Device& device, VkShaderStageFlagBits stage, const ShaderSource& source, const ShaderVariant& variant);

        VkShaderStageFlagBits GetStage() const;

        const std::vector<ShaderResource>& GetResources() const;

        const std::vector<uint32_t>& GetSpirv() const;

        size_t GetHash() const;

    private:
        VkShaderStageFlagBits stage{};

        std::vector<ShaderResource> shaderResources;
        std::vector<uint32_t> spirv;

        size_t hash{ 0 };
    };

}

namespace std
{
    template <>
    struct hash<Vulkan::ShaderSource>
    {
        size_t operator()(const Vulkan::ShaderSource& source) const
        {
            return source.GetHash();
        }
    };
};

namespace std
{
    template <>
    struct hash<Vulkan::ShaderVariant>
    {
        size_t operator()(const Vulkan::ShaderVariant& variant) const
        {
            return variant.GetHash();
        }
    };
};

namespace std
{
    template <>
    struct hash<std::vector<Vulkan::ShaderModule>>
    {
        size_t operator()(const std::vector<Vulkan::ShaderModule>& modules) const
        {
            size_t hash{ 0 };

            for (auto& m : modules)
            {
                HashCombine(hash, m.GetHash());
            }

            return hash;
        }
    };
};
