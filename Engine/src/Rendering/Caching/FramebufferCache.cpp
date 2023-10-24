#include "FramebufferCache.hpp"

namespace Engine
{
    FramebufferCache::FramebufferCache(const Vulkan::Device& device)
        : device(device)
    {
    }

    Vulkan::Framebuffer& FramebufferCache::RequestFramebuffer(const Vulkan::RenderPass& renderPass, const Target& target)
    {
        std::size_t hash{ 0 };
        HashCombine(hash, renderPass);
        HashCombine(hash, target);

        auto it = framebuffers.find(hash);

        if (it != framebuffers.end())
        {
            return *it->second;
        }

        auto [res, _] = framebuffers.emplace(hash, std::make_unique<Vulkan::Framebuffer>(device, renderPass, target.GetViews(), target.GetExtent()));

        return *res->second;
    }
}

