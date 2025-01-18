#pragma once

#include "RenderGraphResource.hpp"

namespace Engine
{
    class RenderGraphAllocator
    {
    public:
        virtual ~RenderGraphAllocator() = default;
        virtual RenderTexture Allocate(const RenderTextureDesc& desc) = 0;
        virtual void Release(const RenderTextureDesc& desc, RenderTexture resource) = 0;
    };
}