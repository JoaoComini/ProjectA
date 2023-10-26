#include "FramebufferCache.hpp"

#include "../RenderTarget.hpp"

namespace std
{
    template <>
    struct hash<Vulkan::RenderPass>
    {
        size_t operator()(const Vulkan::RenderPass& renderPass) const
        {
            return Hash(renderPass.GetHandle());
        }
    };
};

namespace std
{
    template <>
    struct hash<Engine::RenderTarget>
    {
        size_t operator()(const Engine::RenderTarget& target) const
        {
            std::size_t result{ 0 };

            for (auto& view : target.GetViews())
            {
                HashCombine(result, view->GetHandle());
            }

            return result;
        }
    };
};


namespace Engine
{
    FramebufferCache::FramebufferCache(const Vulkan::Device& device)
        : device(device)
    {
    }

    Vulkan::Framebuffer& FramebufferCache::RequestFramebuffer(const Vulkan::RenderPass& renderPass, const RenderTarget& target)
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

    void FramebufferCache::Clear()
    {
        framebuffers.clear();
    }
}

