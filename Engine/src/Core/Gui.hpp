#pragma once

#include "InputEvent.hpp"

namespace Vulkan
{
    class DescriptorPool;
    class CommandBuffer;
    class Image;
    class PipelineLayout;
}

struct ImGuiIO;

namespace Engine
{
    class Window;
    class Texture;

    class Gui
    {

    public:
        Gui(Window& window);
        ~Gui();

        void Begin();
        void Draw(Vulkan::CommandBuffer& commandBuffer);

        bool OnInputEvent(const InputEvent& event);

    private:
        void UploadFonts(ImGuiIO& io);
        void LoadShaders();

        std::unique_ptr<Texture> fontTexture;

        Vulkan::PipelineLayout* pipelineLayout{ nullptr };

        Window& window;
    };
};