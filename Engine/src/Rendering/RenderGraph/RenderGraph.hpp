#pragma once

#include "RenderGraphResource.hpp"
#include "RenderGraphCommand.hpp"
#include "RenderGraphAllocator.hpp"
#include "RenderGraphBuilder.hpp"
#include "RenderGraphContext.hpp"
#include "RenderGraphPass.hpp"

namespace Engine
{
    struct RenderGraphRecord
    {
        std::unique_ptr<RenderGraphPassConcept> pass{ nullptr };

        std::vector<RenderGraphResourceHandle> allocates;
        std::vector<RenderTextureAccess> writes;
        std::vector<RenderTextureAccess> reads;
    };

    class RenderGraph
    {
       
    public:
        RenderGraphResourceHandle Import(const RenderTextureDesc& desc, RenderTexture texture);

        template <typename T>
        void AddPass(RenderGraphPass<T>& pass)
        {
            auto render = std::make_unique<RenderGraphPassRender<T>>(&pass);

            RenderGraphBuilder builder{ resources };
            pass.RecordRenderGraph(builder, context, render->data);

            records.emplace_back(std::move(render), builder.allocates, builder.writes, builder.reads);
        }

        void Compile();
        void Execute(RenderGraphCommand& command, RenderGraphAllocator& allocator);

        RenderGraphContext& GetContext();

    private:
        RenderGraphContext context;
        std::vector<RenderGraphRecord> records;
        std::vector<RenderGraphResourceEntry> resources;

        RenderGraphResourceEntry& GetResourceEntry(RenderGraphResourceHandle handle);
    };
}