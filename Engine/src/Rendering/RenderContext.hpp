#pragma once

#include <memory>
#include <vector>

namespace Vulkan
{
    class Instance;
    class Surface;
    class PhysicalDevice;
    class Device;
    class Swapchain;
    class CommandBuffer;
    class Semaphore;
    class Image;
}

namespace Engine
{
    class Window;
    class RenderTarget;
    class RenderFrame;

    class RenderContext
    {
    public:
        RenderContext(Window& window);
        ~RenderContext();

        Vulkan::CommandBuffer* Begin();
        void End(Vulkan::CommandBuffer& commandBuffer);

        RenderFrame& GetCurrentFrame() const;
        uint32_t GetCurrentFrameIndex() const;
        uint32_t GetFrameCount() const;

        Vulkan::Device& GetDevice();
        Vulkan::PhysicalDevice& GetPhysicalDevice();
        Vulkan::Instance& GetInstance();
        Vulkan::Swapchain& GetSwapchain();

    private:
        void CreateFrames();

        Vulkan::Semaphore& Submit(Vulkan::CommandBuffer& commandBuffer);

        void Present(Vulkan::Semaphore& waitSemaphore);

        bool RecreateSwapchain(bool force = false);

        std::unique_ptr<RenderTarget> CreateRenderTarget(std::unique_ptr<Vulkan::Image>&& swapchainImage);

    private:
        std::unique_ptr<Vulkan::Instance> instance;
        std::unique_ptr<Vulkan::Surface> surface;
        std::unique_ptr<Vulkan::PhysicalDevice> physicalDevice;
        std::unique_ptr<Vulkan::Device> device;
        std::unique_ptr<Vulkan::Swapchain> swapchain;

        uint32_t currentFrameIndex = 0;

        Vulkan::Semaphore* acquireSemaphore;

        std::vector<std::unique_ptr<RenderFrame>> frames;
    };
}