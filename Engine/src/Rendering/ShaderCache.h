#pragma once

#include "Common/Cache.h"
#include "Shader.h"
#include <Shaders/embed.gen.hpp>

#include <tuple>


namespace Engine
{
    class ShaderCache
    {


    public:
        template<typename... Stage>
        auto Get(std::string_view name, const ShaderVariant& variant, Stage... stages)
        {
            return std::tuple{ GetOrCreateShader(name, variant, stages)... };
        }

    private:

        ShaderModule* GetOrCreateShader(std::string_view name, const ShaderVariant& variant, ShaderStage stage)
        {
            auto& source = GetOrCreateSource(name, stage);

            return &shaders.Get(stage, source, variant);
        }

        ShaderSource& GetOrCreateSource(std::string_view name, ShaderStage stage)
        {
            auto path = std::string{ name } + GetStagePrefix(stage) + ".glsl";

            auto it = sources.find(path);

            if (it != sources.end())
            {
                return it->second;
            }

            auto bytes = embed::Shaders::get(path);

            auto source = ShaderSource{ std::vector<uint8_t>{ bytes.begin(), bytes.end() } };
            auto inserted = sources.emplace(path, std::move(source));

            return inserted.first->second;
        }

        static std::string GetStagePrefix(ShaderStage stage)
        {
            switch (stage)
            {
            case ShaderStage::Vertex:
                return ".vert";
            case ShaderStage::Fragment:
                return ".frag";
            }

            return {};
        }

        std::unordered_map<std::string, ShaderSource> sources;
        Cache<ShaderModule> shaders;
    };
}