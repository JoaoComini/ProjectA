#pragma once

#include "RenderGraphResource.hpp"

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
        virtual void BeforeRead(const RenderTextureDesc& desc, const RenderTexture& texture, const RenderTextureAccessInfo& info) = 0;
        virtual void BeforeWrite(const RenderTextureDesc& desc, const RenderTexture& texture, const RenderTextureAccessInfo& info) = 0;

        virtual void BeginPass() = 0;
        virtual void EndPass() = 0;

        virtual void BindUniformBuffer(void* data, uint32_t size, uint32_t set, uint32_t binding) = 0;

        virtual void DrawGeometry(RenderGeometryType type, std::string_view shader) = 0;
        virtual void DrawShadow(glm::vec3 lightDirection) = 0;
        virtual void Blit(std::string_view shader) = 0;
    };

}