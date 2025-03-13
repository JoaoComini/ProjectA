#pragma once

#include "RenderBuffer.h"
#include "RenderTexture.h"

namespace Engine
{
    enum class RenderGeometryType
    {
        Opaque,
        Transparent
    };

    struct DrawGeometrySettings
    {
        RenderGeometryType type{};
        std::string_view shader;
    };

    struct DrawShadowSettings
    {
        glm::vec3 lightDirection{};
        float depthBias{};
        float normalBias{};
    };

    class RenderGraphCommand
    {
    public:
        virtual ~RenderGraphCommand() = default;

        virtual void BeforeRead(const RenderTexture& texture, const RenderTextureDesc& desc, const RenderTextureAccessInfo& info) = 0;
        virtual void BeforeWrite(const RenderTexture& texture, const RenderTextureDesc& desc, const RenderTextureAccessInfo& info) = 0;

        virtual void BeforeRead(const RenderBuffer& buffer, const RenderBufferDesc& dec, const RenderBufferAccessInfo& info) = 0;
        virtual void BeforeWrite(const RenderBuffer& buffer, const RenderBufferDesc& dec, const RenderBufferAccessInfo& info) = 0;

        virtual void BeginPass() = 0;
        virtual void EndPass() = 0;

        virtual void BindUniformBuffer(void* data, uint32_t size, uint32_t set, uint32_t binding) = 0;

        virtual void DrawGeometry(DrawGeometrySettings settings) = 0;
        virtual void DrawShadow(DrawShadowSettings settings) = 0;
        virtual void Blit(std::string_view shader) = 0;
    };

}