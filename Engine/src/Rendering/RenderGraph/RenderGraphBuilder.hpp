#pragma once

#include "RenderGraphResource.hpp"

namespace Engine
{

    class RenderGraph;

    class RenderGraphBuilder
    {
    public:
        RenderGraphBuilder(std::vector<RenderGraphResourceEntry>& resources) : resources(resources) {}

        RenderGraphResourceHandle Allocate(const RenderTextureDesc& desc);
        void Read(RenderGraphResourceHandle handle, RenderTextureAccessInfo info = {});
        void Write(RenderGraphResourceHandle handle, RenderTextureAccessInfo info = {});

    private:
        std::vector<RenderGraphResourceEntry>& resources;

        std::vector<RenderGraphResourceHandle> allocates;
        std::vector<RenderTextureAccess> writes;
        std::vector<RenderTextureAccess> reads;

        friend class RenderGraph;
    };
}