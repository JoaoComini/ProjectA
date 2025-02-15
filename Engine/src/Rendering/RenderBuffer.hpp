#pragma once

#include "Pool/BufferPool.hpp"

namespace Engine
{
    class RenderGraphCommand;
    class RenderGraphAllocator;

    struct RenderBufferDesc
    {
        uint32_t size{};
    };

    struct RenderBufferAccessInfo
    {
        uint32_t set{};
        uint32_t binding{};
    };

    struct RenderBuffer
    {
        using Descriptor = RenderBufferDesc;
        using AccessInfo = RenderBufferAccessInfo;

        using Allocator = RenderGraphAllocator;
        using Command = RenderGraphCommand;

        BufferAllocation allocation{};
    };
}

