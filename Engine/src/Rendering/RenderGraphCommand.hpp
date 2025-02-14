#pragma once

#include "RenderTexture.hpp"

namespace Engine
{
    enum class RenderGeometryType
    {
        Opaque,
        Transparent
    };

    class RenderGraphCommand
    {
    public:
        virtual ~RenderGraphCommand() = default;

        virtual void BeforeRead(const RenderTexture& texture, const RenderTextureDesc& desc, const RenderTextureAccessInfo& info) = 0;
        virtual void BeforeWrite(const RenderTexture& texture, const RenderTextureDesc& desc, const RenderTextureAccessInfo& info) = 0;

        virtual void BeginPass() = 0;
        virtual void EndPass() = 0;

        virtual void BindUniformBuffer(void* data, uint32_t size, uint32_t set, uint32_t binding) = 0;

        virtual void DrawGeometry(RenderGeometryType type, std::string_view shader) = 0;
        virtual void DrawShadow(glm::vec3 lightDirection) = 0;
        virtual void Blit(std::string_view shader) = 0;
    };

}