#pragma once

#include "RenderGraphResource.hpp"
#include "RenderGraphBuilder.hpp"
#include "RenderGraphContext.hpp"

namespace Engine
{
    class RenderGraphBuilder;
    class RenderGraphContext;
    class RenderGraphCommand;

    template<typename Data>
    class RenderGraphPass
    {
    public:
        virtual void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, Data& data) = 0;
        virtual void Render(RenderGraphCommand& command, const Data& data) = 0;
    };

    class RenderGraphPassConcept
    {
    public:
        virtual ~RenderGraphPassConcept() = default;
        virtual void Render(RenderGraphCommand& command) = 0;
    };

    template<typename Data>
    class RenderGraphPassRender : public RenderGraphPassConcept
    {
    public:
        RenderGraphPassRender(RenderGraphPass<Data>* pass) : pass(pass) { }

        void Render(RenderGraphCommand& command) override
        {
            pass->Render(command, data);
        }

        RenderGraphPass<Data>* pass{ nullptr };
        Data data{};
    };
};