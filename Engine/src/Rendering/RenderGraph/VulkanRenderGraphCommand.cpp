#include "VulkanRenderGraphCommand.hpp"

#include "Vulkan/CommandBuffer.hpp"
#include "Vulkan/ResourceCache.hpp"

#include "Rendering/RenderContext.hpp"
#include "Rendering/RenderFrame.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/RenderBatcher.hpp"
#include "Rendering/ShaderCache.hpp"

#include "Resource/ResourceManager.hpp"

#include <Shaders/embed.gen.hpp>

namespace Engine
{
    VulkanRenderGraphCommand::VulkanRenderGraphCommand(
        RenderContext& renderContext,
        RenderBatcher& batcher,
        ShaderCache& shaderCache,
        Vulkan::CommandBuffer& commandBuffer,
        const std::unordered_map<RenderTextureSampler, std::unique_ptr<Vulkan::Sampler>>& samplers
    ) : renderContext(renderContext), batcher(batcher), shaderCache(shaderCache), commandBuffer(commandBuffer), samplers(samplers) { }

    void VulkanRenderGraphCommand::BeforeRead(const RenderTextureDesc&, const RenderTexture& texture, const RenderTextureAccessInfo& info)
    {
        assert(info.type == RenderTextureAccessType::Binding);

        auto* attachment = static_cast<RenderAttachment*>(texture.image);
        auto& view = attachment->GetView();

        auto& scope = attachment->GetScope();
        auto& layout = attachment->GetLayout();

        Vulkan::ImageMemoryBarrierInfo barrier
        {
            .srcStageMask = scope.stage,
            .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            .srcAccessMask = scope.access,
            .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
            .oldLayout = layout,
            .newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        scope.stage = barrier.dstStageMask;
        scope.access = barrier.dstAccessMask;
        layout = barrier.newLayout;

        commandBuffer.ImageMemoryBarrier(view, barrier);

        commandBuffer.BindImage(view, *samplers.at(info.binding.sampler), info.binding.set, info.binding.location, 0);
    }

    void VulkanRenderGraphCommand::BeforeWrite(const RenderTextureDesc&, const RenderTexture& texture, const RenderTextureAccessInfo& info)
    {
        assert(info.type == RenderTextureAccessType::Attachment);

        auto* attachment = static_cast<RenderAttachment*>(texture.image);
        auto& view = attachment->GetView();

        auto& scope = attachment->GetScope();
        auto& layout = attachment->GetLayout();

        Vulkan::ImageMemoryBarrierInfo barrier
        {
            .srcStageMask = scope.stage,
            .srcAccessMask = scope.access,
            .oldLayout = layout,
        };

        assert(info.attachment.aspect != RenderTextureAspect::None);

        if (info.attachment.aspect == RenderTextureAspect::Color)
        {
            barrier.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            colors.push_back({
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = view.GetHandle(),
                .imageLayout = barrier.newLayout,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .clearValue = attachment->GetClearValue(),
            });

            colorFormats.push_back(attachment->GetFormat());
        }
        else
        {
            barrier.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

            depth = std::make_unique<VkRenderingAttachmentInfo>(VkRenderingAttachmentInfo{
                .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
                .imageView = view.GetHandle(),
                .imageLayout = barrier.newLayout,
                .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
                .clearValue = attachment->GetClearValue(),
            });

            depthFormat = attachment->GetFormat();
        }

        scope.stage = barrier.dstStageMask;
        scope.access = barrier.dstAccessMask;
        layout = barrier.newLayout;

        commandBuffer.ImageMemoryBarrier(view, barrier);

        extent = attachment->GetExtent();
    }

    void VulkanRenderGraphCommand::BeginPass()
    {
        commandBuffer.BeginRendering({
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = {
                .offset = { 0, 0 },
                .extent = extent,
            },
            .layerCount = 1,
            .colorAttachmentCount = static_cast<uint32_t>(colors.size()),
            .pColorAttachments = colors.data(),
            .pDepthAttachment = depth.get(),
        });

        commandBuffer.SetPipelineRenderingState({
            .colorAttachmentFormats = colorFormats,
            .depthAttachmentFormat = depthFormat
        });

        commandBuffer.SetViewport({{
            .width = static_cast<float>(extent.width),
            .height = static_cast<float>(extent.height),
            .minDepth = 0.0f,
            .maxDepth = 1.0f,
        }});

        commandBuffer.SetScissor({{
            .extent = extent
        }});
    }

    void VulkanRenderGraphCommand::EndPass()
    {
        commandBuffer.EndRendering();

        colors.clear();
        colorFormats.clear();

        depth = nullptr;
        depthFormat = VK_FORMAT_UNDEFINED;
    }

    void VulkanRenderGraphCommand::BindUniformBuffer(void* data, uint32_t size, uint32_t set, uint32_t binding)
    {
        auto& frame = renderContext.GetCurrentFrame();

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, size);
        allocation.SetData(data);

        commandBuffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), set, binding, 0);
    }

    std::pair<glm::mat4, glm::mat4> GetViewProjection()
    {
        auto [camera, transform] = Renderer::Get().GetMainCamera();

        return { glm::inverse(transform), camera.GetProjection() };
    }

    std::shared_ptr<Material> GetMaterialFromPrimitive(const Primitive& primitive)
    {
        ResourceId materialId = primitive.GetMaterial();

        return ResourceManager::Get().LoadResource<Material>(materialId);
    }

    void VulkanRenderGraphCommand::DrawGeometry(RenderGeometryType type, std::string_view shader)
    {
        Vulkan::VertexInputState vertexInputState{};

        vertexInputState.attributes.resize(3);
        vertexInputState.attributes[0].location = 0;
        vertexInputState.attributes[0].binding = 0;
        vertexInputState.attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputState.attributes[0].offset = offsetof(Vertex, position);

        vertexInputState.attributes[1].location = 1;
        vertexInputState.attributes[1].binding = 0;
        vertexInputState.attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputState.attributes[1].offset = offsetof(Vertex, normal);

        vertexInputState.attributes[2].location = 2;
        vertexInputState.attributes[2].binding = 0;
        vertexInputState.attributes[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputState.attributes[2].offset = offsetof(Vertex, uv);

        vertexInputState.bindings.resize(1);
        vertexInputState.bindings[0].binding = 0;
        vertexInputState.bindings[0].stride = sizeof(Vertex);
        vertexInputState.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        commandBuffer.SetVertexInputState(vertexInputState);

        switch(type) {
        case RenderGeometryType::Opaque:
            DrawOpaques(shader);
            break;
        case RenderGeometryType::Transparent:
            DrawTransparents();
            break;
        }
    }

    struct LightPushConstant
    {
        glm::vec3 direction;
        float depthBias;
        float normalBias;
        glm::mat4 viewProjection;
    };

    struct ModelUniform
    {
        glm::mat4 localToWorldMatrix;
    };

    struct PbrPushConstant
    {
        glm::vec4 albedoColor;
        float metallicFactor;
        float roughnessFactor;
        float alphaCutoff;
    };

    void VulkanRenderGraphCommand::DrawShadow(glm::vec3 lightDirection)
    {
        auto shaders = shaderCache.Get("shadowmap", {}, ShaderStage::Vertex);

        auto& layout = renderContext.GetDevice().GetResourceCache().RequestPipelineLayout({ &std::get<0>(shaders) });

        commandBuffer.BindPipelineLayout(layout);

        Vulkan::VertexInputState vertexInputState{};

        vertexInputState.attributes.resize(2);
        vertexInputState.attributes[0].location = 0;
        vertexInputState.attributes[0].binding = 0;
        vertexInputState.attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputState.attributes[0].offset = offsetof(Vertex, position);

        vertexInputState.attributes[1].location = 1;
        vertexInputState.attributes[1].binding = 0;
        vertexInputState.attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputState.attributes[1].offset = offsetof(Vertex, normal);

        vertexInputState.bindings.resize(1);
        vertexInputState.bindings[0].binding = 0;
        vertexInputState.bindings[0].stride = sizeof(Vertex);
        vertexInputState.bindings[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        commandBuffer.SetVertexInputState(vertexInputState);

        auto settings = Renderer::Get().GetSettings();

        auto view = glm::lookAt(lightDirection, glm::vec3{ 0 }, glm::vec3{ 0, 1, 0 });
        auto projection = glm::ortho(-25.f, 25.f, -25.f, 25.f, -25.f, 25.f);

        LightPushConstant pushConstant
        {
            .direction = -glm::normalize(glm::vec3(view[2])),
            .depthBias = -settings.shadow.depthBias * 50.f / 2048.f,
            .normalBias = -settings.shadow.normalBias * 50.f / 2048.f,
            .viewProjection = projection * view,
        };

        commandBuffer.PushConstants(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(LightPushConstant), &pushConstant);

        auto& frame = renderContext.GetCurrentFrame();

        for (const auto& opaque : batcher.GetOpaques())
        {
            ModelUniform uniform{
                .localToWorldMatrix = opaque.transform
            };

            auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ModelUniform));
            allocation.SetData(&uniform);

            opaque.primitive->Draw(commandBuffer);
        }
    }

    void VulkanRenderGraphCommand::DrawOpaques(std::string_view shader)
    {
        for (const auto& opaque : batcher.GetOpaques())
        {
            UpdateModelUniform(commandBuffer, opaque.transform, *opaque.material);

            SetupShader(shader, *opaque.material);

            opaque.primitive->Draw(commandBuffer);
        }
    }

    void VulkanRenderGraphCommand::DrawTransparents()
    {
        Vulkan::ColorBlendState colorBlendState{};
        colorBlendState.attachments.push_back({
            .blendEnable = VK_TRUE,
        });

        commandBuffer.SetColorBlendState(colorBlendState);

        auto& transparents = batcher.GetTransparents();

        // Draw transparent objects back to front
        for (auto transparent = transparents.rbegin(); transparent != transparents.rend(); transparent++)
        {
            auto material = GetMaterialFromPrimitive(*transparent->primitive);

            UpdateModelUniform(commandBuffer, transparent->transform, *transparent->material);

            transparent->primitive->Draw(commandBuffer);
        }
    }

    void VulkanRenderGraphCommand::UpdateModelUniform(Vulkan::CommandBuffer& commandBuffer, const glm::mat4& matrix, const Material& material)
    {
        auto& frame = renderContext.GetCurrentFrame();

        ModelUniform uniform{};
        uniform.localToWorldMatrix = matrix;

        auto allocation = frame.RequestBufferAllocation(Vulkan::BufferUsageFlags::UNIFORM, sizeof(ModelUniform));
        allocation.SetData(&uniform);

        commandBuffer.BindBuffer(allocation.GetBuffer(), allocation.GetOffset(), allocation.GetSize(), 0, 1, 0);

        if (auto albedo = ResourceManager::Get().LoadResource<Texture>(material.GetAlbedoTexture()))
        {
            commandBuffer.BindImage(albedo->GetImageView(), albedo->GetSampler(), 0, 2, 0);
        }

        if (auto normal = ResourceManager::Get().LoadResource<Texture>(material.GetNormalTexture()))
        {
            commandBuffer.BindImage(normal->GetImageView(), normal->GetSampler(), 0, 3, 0);
        }

        if (auto metallicRoughness = ResourceManager::Get().LoadResource<Texture>(material.GetMetallicRoughnessTexture()))
        {
            commandBuffer.BindImage(metallicRoughness->GetImageView(), metallicRoughness->GetSampler(), 0, 4, 0);
        }
    }

    void VulkanRenderGraphCommand::SetupShader(std::string_view name, const Material& material)
    {
        auto& variant = material.GetShaderVariant();

        auto shaders = shaderCache.Get(name, variant, ShaderStage::Vertex, ShaderStage::Fragment);

        auto& layout = renderContext.GetDevice().GetResourceCache().RequestPipelineLayout({&std::get<0>(shaders), &std::get<1>(shaders)});

        commandBuffer.BindPipelineLayout(layout);

        if (layout.HasShaderResource(ShaderResourceType::PushConstant))
        {
            PbrPushConstant pushConstant
            {
                .albedoColor = material.GetAlbedoColor(),
                .metallicFactor = material.GetMetallicFactor(),
                .roughnessFactor = material.GetRoughnessFactor(),
                .alphaCutoff = material.GetAlphaCutoff(),
            };

            commandBuffer.PushConstants(VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PbrPushConstant), &pushConstant);
        }

        commandBuffer.FlushDescriptorSet(0);
    }

    void VulkanRenderGraphCommand::Blit(std::string_view shader)
    {
        commandBuffer.SetRasterizationState({ VK_CULL_MODE_FRONT_BIT });

        auto shaders = shaderCache.Get(shader, {}, ShaderStage::Vertex, ShaderStage::Fragment);

        auto& layout = renderContext.GetDevice().GetResourceCache().RequestPipelineLayout({ &std::get<0>(shaders), &std::get<1>(shaders) });

        commandBuffer.BindPipelineLayout(layout);

        commandBuffer.FlushDescriptorSet(0);

        commandBuffer.Draw(3, 1, 0, 0);
    }
}