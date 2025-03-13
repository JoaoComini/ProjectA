#pragma once

#include "RenderGraphResource.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphContext.h"

namespace Engine
{
    class RenderGraphBuilder;
    class RenderGraphContext;
    class RenderGraphCommand;

    template<typename Data, typename Command>
    class RenderGraphPass
    {
    public:
        virtual ~RenderGraphPass() = default;

        virtual void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, Data& data) = 0;
        virtual void Render(Command& command, const Data& data) = 0;
    };

    class RenderGraphPassConcept
    {
    public:
        virtual ~RenderGraphPassConcept() = default;
        virtual void Render(void* command) = 0;
    };

    template<typename Data, typename Command>
    class RenderGraphPassRender : public RenderGraphPassConcept
    {
    public:
        RenderGraphPassRender(RenderGraphPass<Data, Command>* pass) : pass(pass) { }

        void Render(void* command) override
        {
            auto typed = static_cast<Command*>(command);

            pass->Render(*typed, data);
        }

        RenderGraphPass<Data, Command>* pass{ nullptr };
        Data data{};
    };
};