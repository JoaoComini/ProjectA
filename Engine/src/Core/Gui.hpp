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
    class RenderContext;
    class RenderAttachment;

    class Gui
    {

    public:
        Gui(Window& window, RenderContext& renderContext);
        ~Gui();

        void Begin();
        void Draw(Vulkan::CommandBuffer& commandBuffer, RenderAttachment& target);

        bool OnInputEvent(const InputEvent& event);

    private:
        void UploadFonts(ImGuiIO& io);
        void LoadShaders();

        std::unique_ptr<Texture> fontTexture;

        Vulkan::PipelineLayout* pipelineLayout{ nullptr };

        Window& window;
        RenderContext& renderContext;
    };
};