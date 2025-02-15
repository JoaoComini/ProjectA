#pragma once

#include "RenderTexture.hpp"
#include "RenderBuffer.hpp"

namespace Engine
{
    class RenderGraphAllocator
    {
    public:
        virtual ~RenderGraphAllocator() = default;
        virtual RenderTexture Allocate(const RenderTextureDesc& desc) = 0;
        virtual void Free(RenderTexture resource, const RenderTextureDesc& desc) = 0;

        virtual RenderBuffer Allocate(const RenderBufferDesc& desc) = 0;
        virtual void Free(RenderBuffer resource, const RenderBufferDesc& desc) = 0;
    };
}