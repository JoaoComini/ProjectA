#include <catch2/catch_test_macros.hpp>

#include <Rendering/RenderGraph/RenderGraph.h>

using namespace Engine;

struct FirstPassData
{
    RenderGraphResourceHandle target;
};

struct SecondPassData
{

};

struct BackbufferData
{
    RenderGraphResourceHandle handle;
};

class FirstPass : public RenderGraphPass<FirstPassData>
{

public:
    void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, FirstPassData& data) override
    {
        data.target = builder.Allocate(RenderTextureDesc{});

        builder.Write(data.target);

        context.Add<FirstPassData>(data);
    }

    void Render(RenderGraphCommand& command, const FirstPassData& data) override
    {
        rendered = true;
    }

    bool rendered = false;
};

class SecondPass : public RenderGraphPass<SecondPassData>
{

public:
    void RecordRenderGraph(RenderGraphBuilder& builder, RenderGraphContext& context, SecondPassData& data) override
    {
        auto& first = context.Get<FirstPassData>();
        auto& backbuffer = context.Get<BackbufferData>();

        builder.Read(first.target);
        builder.Write(backbuffer.handle);
    }

    void Render(RenderGraphCommand& command, const SecondPassData& data) override
    {
        rendered = true;
    }

    bool rendered = false;
};

class MockAllocator : public RenderGraphAllocator
{
public:
    virtual RenderTexture Allocate(const RenderTextureDesc& desc) override
    {
        allocations += 1;
        return {};
    }

    virtual void Release(const RenderTextureDesc& desc, RenderTexture resource) override
    {
        releases += 1;
    }

    int allocations = 0;
    int releases = 0;
};

class MockCommand : public RenderGraphCommand
{
public:
    void BeforeRead(const RenderTextureDesc& desc, RenderTexture& texture) override
    {

    }

    void BeforeWrite(const RenderTextureDesc& desc, RenderTexture& texture) override
    {

    }
};


TEST_CASE("it should compile and execute", "[RenderGraph]")
{
    MockAllocator allocator;
    MockCommand command;
    RenderGraph graph;

    auto& context = graph.GetContext();
    auto& backbuffer = context.Add<BackbufferData>();
    backbuffer.handle = graph.AddRenderTexture({}, nullptr);

    FirstPass first;
    graph.AddPass(first);

    SecondPass second;
    graph.AddPass(second);

    graph.Compile();
    graph.Execute(command, allocator);

    REQUIRE(allocator.allocations == 1);
    REQUIRE(allocator.releases == 1);
    REQUIRE(first.rendered);
    REQUIRE(second.rendered);
}

TEST_CASE("it should allocate a resources", "[RenderGraph::Builder]")
{
    std::vector<RenderGraphResourceEntry> entries;

    RenderGraphBuilder builder{ entries };

    auto first = builder.Allocate(RenderTextureDesc{});
    auto second = builder.Allocate(RenderTextureDesc{});

    REQUIRE(first == 0);
    REQUIRE(second == 1);
}

TEST_CASE("it should save data", "[RenderGraph::Context]")
{
    struct CustomData
    {
        int test{ 10 };
    };

    RenderGraphContext context;

    const int& primitive = context.Add<int>(2);
    CustomData& structure = context.Add<CustomData>(12);

    REQUIRE(primitive == 2);
    REQUIRE(structure.test == 12);

}

TEST_CASE("it should retrieve data", "[RenderGraph::Context]")
{
    struct CustomData
    {
        int test{ 10 };
    };

    RenderGraphContext context;

    context.Add<int>(2);
    context.Add<CustomData>(12);

    auto& primitive = context.Get<int>();
    auto& structure = context.Get<CustomData>();

    REQUIRE(primitive == 2);
    REQUIRE(structure.test == 12);
}
