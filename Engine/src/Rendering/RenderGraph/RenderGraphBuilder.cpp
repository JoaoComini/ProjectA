#include "RenderGraphBuilder.hpp"

namespace Engine
{

    RenderGraphResourceHandle RenderGraphBuilder::Allocate(const RenderTextureDesc& desc)
    {
        auto handle = resources.size();
        resources.push_back({ RenderGraphResourceType::Transient, desc, {}, nullptr });

        allocates.push_back(handle);
        return handle;
    }

    void RenderGraphBuilder::Read(RenderGraphResourceHandle resource, RenderTextureAccessInfo info)
    {
        reads.push_back({resource, info});
    }

    void RenderGraphBuilder::Write(RenderGraphResourceHandle resource, RenderTextureAccessInfo info)
    {
        writes.push_back({ resource, info });
    }
}