#pragma once

#include "Common/ScopedEnum.h"
#include "Common/Hash.h"

#include "RenderAttachment.h"

namespace Engine
{
    class RenderGraphCommand;
    class RenderGraphAllocator;

    enum class RenderTextureFormat
    {
        Linear = 1 << 0,
        sRGB = 1 << 1,
        HDR = 1 << 2,
        Depth = 1 << 3
    };
    template <> struct has_flags<RenderTextureFormat> : std::true_type {};

    enum class RenderTextureUsage {
        TransferSrc = 1 << 0,
        TransferDst = 1 << 1,
        Transfer = TransferSrc | TransferDst,
        RenderTarget = 1 << 3,
        Sampled = 1 << 4,
    };
    template <> struct has_flags<RenderTextureUsage> : std::true_type {};


    enum class RenderTextureAccessType
    {
        Binding,
        Attachment
    };
    template <> struct has_flags<RenderTextureAccessType> : std::true_type {};

    enum class RenderGraphPipelineStage {
        VertexShader = 1 << 0,
        FragmentShader = 1 << 1,
        ComputeShader = 1 << 2,
    };
    template <> struct has_flags<RenderGraphPipelineStage> : std::true_type {};

    enum class RenderTextureAspect
    {
        None = 0,
        Color,
        Depth,
    };

    struct RenderTextureAttachmentAccess
    {
        RenderTextureAspect aspect{ RenderTextureAspect::None };
    };

    enum class RenderTextureSampler
    {
        Point,
        Bilinear,
        Shadow,
    };

    struct RenderTextureBindingAccess
    {
        uint32_t set{ 0 };
        uint32_t location{ 0 };
        RenderTextureSampler sampler{ RenderTextureSampler::Point };
    };

    struct RenderTextureDesc
    {
        uint32_t width{ 0 };
        uint32_t height{ 0 };
        RenderTextureFormat format{};
        RenderTextureUsage usage{};

        bool operator==(RenderTextureDesc const&) const = default;
    };

    struct RenderTextureAccessInfo
    {
        RenderTextureAccessType type{ RenderTextureAccessType::Attachment };
        union
        {
            RenderTextureAttachmentAccess attachment;
            RenderTextureBindingAccess binding;
        };
    };

    struct RenderTexture
    {
        using Descriptor = RenderTextureDesc;
        using AccessInfo = RenderTextureAccessInfo;

        using Allocator = RenderGraphAllocator;
        using Command = RenderGraphCommand;

        RenderAttachment* attachment{ nullptr };
    };
}

template <>
struct std::hash<Engine::RenderTextureDesc>
{
    size_t operator()(const Engine::RenderTextureDesc& desc) const noexcept
    {
        size_t hash{ 0 };

        HashCombine(hash, desc.width);
        HashCombine(hash, desc.height);
        HashCombine(hash, desc.format);
        HashCombine(hash, desc.usage);

        return hash;
    }
};;