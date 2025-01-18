#pragma once

#include "RenderGraphCommand.hpp"
#include "Rendering/RenderBatcher.hpp"

namespace Vulkan
{
    class CommandBuffer;
    class Sampler;
}

namespace Engine
{
    class RenderContext;
    class RenderBatcher;
    class ShaderVariant;
    class ShaderCache;

    class VulkanRenderGraphCommand : public RenderGraphCommand
    {
    public:
        VulkanRenderGraphCommand(
            RenderContext& renderContext,
            RenderBatcher& batcher,
            ShaderCache& shaderCache,
            Vulkan::CommandBuffer& commandBuffer,
            const std::unordered_map<RenderTextureSampler, std::unique_ptr<Vulkan::Sampler>>& samplers
        );

        void BeforeRead(const RenderTextureDesc& desc, const RenderTexture& texture, const RenderTextureAccessInfo& info) override;
        void BeforeWrite(const RenderTextureDesc& desc, const RenderTexture& texture, const RenderTextureAccessInfo& info) override;

        void BeginPass() override;
        void EndPass() override;

        void DrawGeometry(RenderGeometryType type, std::string_view shader) override;
        void Blit(std::string_view shader);

    private:
        void DrawOpaques(std::string_view shader);
        void DrawTransparents();

        void UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& matrix, const Material& material);

        void SetupShader(std::string_view shader, const Material& material);

        RenderContext& renderContext;
        RenderBatcher& batcher;
        ShaderCache& shaderCache;
        Vulkan::CommandBuffer& commandBuffer;

        const std::unordered_map<RenderTextureSampler, std::unique_ptr<Vulkan::Sampler>>& samplers;

        std::vector<VkRenderingAttachmentInfo> colors;
        std::vector<VkFormat> colorFormats;

        std::unique_ptr<VkRenderingAttachmentInfo> depth;
        VkFormat depthFormat{ VK_FORMAT_UNDEFINED };

        VkExtent2D extent{};
    };

}