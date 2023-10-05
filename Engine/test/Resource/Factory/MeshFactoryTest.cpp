#include <catch2/catch_test_macros.hpp>

#include "Core/Window.hpp"
#include "Vulkan/Instance.hpp"
#include "Resource/Factory/MeshFactory.hpp"

using namespace Engine;

TEST_CASE("it should create and retrieve a mesh", "[MeshFactory]")
{
    std::unique_ptr<Window> window = WindowBuilder()
        .Width(800)
        .Height(600)
        .Build();

    std::unique_ptr<Vulkan::Instance> instance = Vulkan::InstanceBuilder().Build();;
    std::unique_ptr<Vulkan::Surface> surface = window->CreateSurface(*instance);
    std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, *surface);
    std::unique_ptr<Vulkan::Device> device = std::make_unique<Vulkan::Device>(*instance, *physicalDevice);

    MeshFactory factory{ *device };

    std::vector<uint8_t> image(1600, 1);

    MeshSpec spec =
    {
        .material = ResourceId{},
        .vertices = {Vertex{}, Vertex{}},
        .indices = {1, 2, 3, 4},
        .indexType = VK_INDEX_TYPE_UINT32
    };

    factory.Create("test.mesh", spec);
    factory.Load("test.mesh");

    device->WaitIdle();
}