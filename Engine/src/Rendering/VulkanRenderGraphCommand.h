#pragma once

#include "RenderGraphCommand.h"
#include "Rendering/RenderBatcher.h"

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

    class VulkanRenderGraphCommand final : public RenderGraphCommand
    {
    public:
        VulkanRenderGraphCommand(
            RenderContext& renderContext,
            RenderBatcher& batcher,
            ShaderCache& shaderCache,
            Vulkan::CommandBuffer& commandBuffer,
            const std::unordered_map<RenderTextureSampler, std::unique_ptr<Vulkan::Sampler>>& samplers
        );

        void BeforeRead(const RenderTexture& texture, const RenderTextureDesc& desc, const RenderTextureAccessInfo& info) override;
        void BeforeWrite(const RenderTexture& texture, const RenderTextureDesc& desc, const RenderTextureAccessInfo& info) override;

        void BeforeRead(const RenderBuffer &buffer, const RenderBufferDesc &desc, const RenderBufferAccessInfo &info) override;
        void BeforeWrite(const RenderBuffer &buffer, const RenderBufferDesc &desc, const RenderBufferAccessInfo &info) override;

        void BeginPass() override;
        void EndPass() override;

        void BindUniformBuffer(void* data, uint32_t size, uint32_t set, uint32_t binding) override;

        void DrawGeometry(DrawGeometrySettings settings) override;
        void DrawShadow(DrawShadowSettings settings) override;
        void Blit(std::string_view shader) override;

    private:
        void DrawOpaques(std::string_view shader);
        void DrawTransparents(std::string_view shader);

        void UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& matrix, const Material& material);

        void SetupShader(std::string_view shader, const Material& material);

    public:


    private:
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