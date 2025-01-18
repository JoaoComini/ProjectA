#pragma once

#include "Common/Hash.hpp"
#include "Common/ScopedEnum.hpp"

namespace Engine
{
    enum class ShaderResourceType
    {
        Input,
        Output,
        ImageSampler,
        BufferUniform,
        PushConstant
    };

    enum class ShaderStage
    {
        Vertex = 1 << 0,
        Fragment = 1 << 1,
    };
    template <> struct has_flags<ShaderStage> : std::true_type {};

    struct ShaderResource
    {
        ShaderResourceType type;
        ShaderStage stages;
        std::string name;

        uint32_t set;
        uint32_t binding;
        uint32_t location;
        uint32_t vecSize;
        uint32_t columns;
        uint32_t arraySize;
        uint32_t size;
    };

    class ShaderSource
    {
    public:
        ShaderSource(const std::vector<uint8_t>& bytes);

        const std::string& GetSource() const;

        size_t GetHash() const;
    private:
        std::string source;

        size_t hash{ 0 };
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

    class Shader
    {
    public:
        Shader(ShaderStage stage, const ShaderSource& source, const ShaderVariant& variant);

        ShaderStage GetStage() const;

        const std::vector<ShaderResource>& GetResources() const;

        const std::vector<uint32_t>& GetSpirv() const;

        size_t GetHash() const;

    private:
        ShaderStage stage{};

        std::vector<ShaderResource> shaderResources;
        std::vector<uint32_t> spirv;

        size_t hash{ 0 };
    };
}

namespace std
{
    template <>
    struct hash<Engine::ShaderSource>
    {
        size_t operator()(const Engine::ShaderSource& source) const
        {
            return source.GetHash();
        }
    };
};

namespace std
{
    template <>
    struct hash<Engine::ShaderVariant>
    {
        size_t operator()(const Engine::ShaderVariant& variant) const
        {
            return variant.GetHash();
        }
    };
};

namespace std
{
    template <>
    struct hash<std::vector<Engine::Shader*>>
    {
        size_t operator()(const std::vector<Engine::Shader*>& modules) const
        {
            size_t hash{ 0 };

            for (auto& m : modules)
            {
                HashCombine(hash, m->GetHash());
            }

            return hash;
        }
    };
};