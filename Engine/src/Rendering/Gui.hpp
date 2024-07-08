#pragma once

#include "Common/Singleton.hpp"

namespace Vulkan
{
    class DescriptorPool;
    class CommandBuffer;
}

namespace Engine
{
    class RenderContext;
    class Window;

    class Gui : public Singleton<Gui>
    {

    public:
        Gui(Window& window);
        ~Gui();

        void Begin();
        void End(Vulkan::CommandBuffer& commandBuffer);

    private:
        std::unique_ptr<Vulkan::DescriptorPool> descriptorPool;
        Window& window;
    };
};