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
        explicit ShaderSource(const std::vector<uint8_t>& bytes);

        [[nodiscard]] const std::string& GetSource() const;

        [[nodiscard]] size_t GetHash() const;
    private:
        std::string source;
        size_t hash{ 0 };
    };

    class ShaderVariant
    {
    public:
        void AddDefine(const std::string& define);
        [[nodiscard]] const std::string& GetPreamble() const;

        [[nodiscard]] size_t GetHash() const;

    private:
        std::string preamble{};
        size_t hash{ 0 };
    };

    class ShaderModule
    {
    public:
        ShaderModule(ShaderStage stage, const ShaderSource& source, const ShaderVariant& variant);

        [[nodiscard]] ShaderStage GetStage() const;

        [[nodiscard]] const std::vector<ShaderResource>& GetResources() const;

        [[nodiscard]] const std::vector<uint32_t>& GetSpirv() const;

        [[nodiscard]] size_t GetHash() const;

    private:
        ShaderStage stage{};

        std::vector<ShaderResource> shaderResources;
        std::vector<uint32_t> spirv;

        size_t hash{ 0 };
    };
}

template <>
struct std::hash<Engine::ShaderSource>
{
    size_t operator()(const Engine::ShaderSource& source) const noexcept
    {
        return source.GetHash();
    }
};

template <>
struct std::hash<Engine::ShaderVariant>
{
    size_t operator()(const Engine::ShaderVariant& variant) const noexcept
    {
        return variant.GetHash();
    }
};

template <>
struct std::hash<std::vector<Engine::ShaderModule*>>
{
    size_t operator()(const std::vector<Engine::ShaderModule*>& modules) const noexcept
    {
        size_t hash{ 0 };

        for (auto& m : modules)
        {
            HashCombine(hash, m->GetHash());
        }

        return hash;
    }
};;