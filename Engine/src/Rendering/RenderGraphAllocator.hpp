#pragma once

#include "RenderTexture.hpp"

namespace Engine
{
    class RenderGraphAllocator
    {
    public:
        virtual ~RenderGraphAllocator() = default;
        virtual RenderTexture Allocate(const RenderTextureDesc& desc) = 0;
        virtual void Free(RenderTexture resource, const RenderTextureDesc& desc) = 0;
    };
}