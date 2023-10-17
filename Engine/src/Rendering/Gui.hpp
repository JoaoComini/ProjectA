#pragma once

#include "Core/Window.hpp"

#include "Vulkan/Device.hpp"
#include "Vulkan/PhysicalDevice.hpp"
#include "Vulkan/Instance.hpp"
#include "Vulkan/RenderPass.hpp"
#include "Vulkan/DescriptorPool.hpp"

#include "Common/Singleton.hpp"

namespace Engine
{

    class Gui : public Singleton<Gui>
    {

    public:
        Gui(std::unique_ptr<Vulkan::DescriptorPool> descriptorPool, Window& window);
        ~Gui();

        static void Setup(Vulkan::Instance& instance, Vulkan::Device& device, Vulkan::PhysicalDevice& physicalDevice, Window& window);

        void Begin();
        void End(Vulkan::CommandBuffer& commandBuffer);

    private:

        std::unique_ptr<Vulkan::DescriptorPool> descriptorPool;
        Window& window;
    };
};