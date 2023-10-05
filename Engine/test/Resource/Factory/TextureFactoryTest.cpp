#include <catch2/catch_test_macros.hpp>


#include "Core/Window.hpp"
#include "Vulkan/Instance.hpp"
#include "Resource/Factory/TextureFactory.hpp"

using namespace Engine;

TEST_CASE("it should create and retrieve a texture", "[TextureFactory]")
{
    std::unique_ptr<Window> window = WindowBuilder()
        .Width(800)
        .Height(600)
        .Build();

    std::unique_ptr<Vulkan::Instance> instance = Vulkan::InstanceBuilder().Build();;
    std::unique_ptr<Vulkan::Surface> surface = window->CreateSurface(*instance);
    std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice = Vulkan::PhysicalDevicePicker::PickBestSuitable(*instance, *surface);
    std::unique_ptr<Vulkan::Device> device = std::make_unique<Vulkan::Device>(*instance, *physicalDevice);
    
    TextureFactory factory{ *device };

    std::vector<uint8_t> image(1600, 1);

    TextureSpec spec =
    {
        .width = 20,
        .height = 20,
        .component = 4,
        .image = image
    };

    factory.Create("test.texture", spec);
    factory.Load("test.texture");

    device->WaitIdle();
}