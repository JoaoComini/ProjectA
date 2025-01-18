#include "RenderGraph.hpp"

#include "RenderGraphPass.hpp"

namespace Engine
{
    RenderGraphResourceHandle RenderGraph::Import(const RenderTextureDesc& desc, RenderTexture texture)
    {
        auto handle = resources.size();
        resources.push_back({ RenderGraphResourceType::External, desc, texture, nullptr });

        return handle;
    }

    void RenderGraph::Compile()
    {
        for (auto& record : records)
        {
            for (auto& access: record.reads)
            {
                auto& resource = GetResourceEntry(access.handle);

                resource.last = &record;
            }

            for (auto& access : record.writes)
            {
                auto& resource = GetResourceEntry(access.handle);

                resource.last = &record;
            }
        }
    }

    void RenderGraph::Execute(RenderGraphCommand& command, RenderGraphAllocator& allocator)
    {
        for (auto& record : records)
        {
            for (auto handle : record.allocates)
            {
                auto& entry = GetResourceEntry(handle);

                entry.resource = allocator.Allocate(entry.descriptor);
            }

            for (auto& access: record.reads)
            {
                auto& entry = GetResourceEntry(access.handle);

                command.BeforeRead(entry.descriptor, entry.resource, access.info);
            }

            for (auto& access: record.writes)
            {
                auto& entry = GetResourceEntry(access.handle);

                command.BeforeWrite(entry.descriptor, entry.resource, access.info);
            }

            command.BeginPass();

            record.pass->Render(command);

            command.EndPass();

            for (const auto& entry : resources)
            {
                if (entry.last == &record && entry.type == RenderGraphResourceType::Transient)
                {
                    allocator.Release(entry.descriptor, entry.resource);
                }
            }
        }
    }

    RenderGraphContext& RenderGraph::GetContext()
    {
        return context;
    }

    RenderGraphResourceEntry& RenderGraph::GetResourceEntry(RenderGraphResourceHandle handle)
    {
        assert(handle <= resources.size());

        return resources[handle];
    }
}