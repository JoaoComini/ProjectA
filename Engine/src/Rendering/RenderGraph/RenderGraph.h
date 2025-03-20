#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphContext.h"
#include "RenderGraphPass.h"

namespace Engine
{
    struct RenderGraphRecord
    {
        std::unique_ptr<RenderGraphPassConcept> pass{ nullptr };

        std::vector<RenderGraphResourceHandleBase> allocates;
        std::vector<RenderGraphResourceAccess> writes;
        std::vector<RenderGraphResourceAccess> reads;
    };

    class RenderGraph
    {
       
    public:
        template<typename T>
        RenderGraphResourceHandle<T> Import(T&& resource, const typename T::Descriptor& desc)
        {
            auto handle = RenderGraphResourceHandle<T>(resources.size());
            resources.push_back({ RenderGraphResource::Type::Imported, std::forward<T>(resource), desc });

            return handle;
        }

        template <typename Data, typename Command>
        void AddPass(RenderGraphPass<Data, Command>& pass)
        {
            auto render = std::make_unique<RenderGraphPassRender<Data, Command>>(&pass);

            RenderGraphBuilder builder{ resources };
            pass.RecordRenderGraph(builder, context, render->data);

            records.emplace_back(std::move(render), std::move(builder.allocates), std::move(builder.writes), std::move(builder.reads));
        }

        void Compile()
        {
            for (auto& record : records)
            {
                for (auto& access: record.reads)
                {
                    auto& resource = GetResource(access.GetHandle());

                    resource.SetLastRecord(&record);
                }

                for (auto& access : record.writes)
                {
                    auto& resource = GetResource(access.GetHandle());

                    resource.SetLastRecord(&record);
                }
            }
        }

        template<typename Command, typename Allocator>
        void Execute(Command& command, Allocator& allocator)
        {
            for (auto& record : records)
            {
                for (auto handle : record.allocates)
                {
                    auto& resource = GetResource(handle);

                    resource.Allocate(&allocator);
                }

                for (auto& access: record.reads)
                {
                    auto& resource = GetResource(access.GetHandle());

                    access.BeforeRead(&command, resource);
                }

                for (auto& access: record.writes)
                {
                    auto& resource = GetResource(access.GetHandle());

                    access.BeforeWrite(&command, resource);
                }

                command.BeginPass();

                record.pass->Render(&command);

                command.EndPass();

                for (auto& resource : resources)
                {
                    if (resource.GetLastRecord() == &record && resource.GetType() == RenderGraphResource::Type::Transient)
                    {
                        resource.Free(&allocator);
                    }
                }
            }
        }

        RenderGraphContext& GetContext()
        {
            return context;
        }

    private:
        RenderGraphContext context;
        std::vector<RenderGraphRecord> records;
        std::vector<RenderGraphResource> resources;

        RenderGraphResource& GetResource(RenderGraphResourceHandleBase handle)
        {
            assert(handle.GetId() <= resources.size());

            return resources[handle.GetId()];
        }
    };
}