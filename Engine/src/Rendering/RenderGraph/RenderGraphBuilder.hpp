#pragma once

#include "RenderGraphResource.hpp"

namespace Engine
{

    class RenderGraph;

    class RenderGraphBuilder
    {
    public:
        explicit RenderGraphBuilder(std::vector<RenderGraphResource>& resources) : resources(resources) {}

        template <typename T>
        RenderGraphResourceHandle<T> Allocate(const typename T::Descriptor& desc)
        {
            auto handle = RenderGraphResourceHandle<T>(resources.size());
            resources.emplace_back(RenderGraphResource::Type::Transient, T{}, desc);

            allocates.push_back(handle);
            return handle;
        }

        template <typename T>
        void Read(RenderGraphResourceHandle<T> handle, const typename T::AccessInfo& info = {})
        {
            reads.emplace_back(handle, info);
        }

        template <typename T>
        void Write(RenderGraphResourceHandle<T> handle, const typename T::AccessInfo& info = {})
        {
            writes.emplace_back(handle, info);
        }

    private:
        std::vector<RenderGraphResource>& resources;

        std::vector<RenderGraphResourceHandleBase> allocates;
        std::vector<RenderGraphResourceAccess> reads;
        std::vector<RenderGraphResourceAccess> writes;

        friend class RenderGraph;
    };
}