#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <string.h>
#include <memory>

#include "Instance.hpp"
#include "Surface.hpp"
#include "Queue.hpp"
#include "Details.hpp"

namespace Vulkan
{
    struct SurfaceSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class PhysicalDevice
    {
    public:
        PhysicalDevice() = default;
        PhysicalDevice(VkPhysicalDevice handle, const Surface &surface);

        uint32_t FindQueueIndex(Queue::Type type) const;
        uint32_t FindPresentQueueIndex() const;

        SurfaceSupportDetails GetSurfaceSupportDetails() const;
        VkPhysicalDeviceProperties GetProperties() const;

        VkPhysicalDevice GetHandle() const;

    private:
        uint32_t FindFirstQueueIndex(VkQueueFlagBits flag) const;

        VkPhysicalDevice handle;
        const Surface& surface;
        std::vector<VkQueueFamilyProperties> families;

        friend class PhysicalDevicePicker;
        friend class Device;
        friend class SwapchainBuilder;
    };

    class PhysicalDevicePicker
    {
    public:
        virtual ~PhysicalDevicePicker() = default;

        static std::unique_ptr<PhysicalDevice> PickBestSuitable(const Instance &instance, const Surface& surface);

    private:
        static bool IsDeviceSuitable(PhysicalDevice device);
        static bool HasSuitableQueueFamily(PhysicalDevice device);
        static bool HasExtensionsSupport(PhysicalDevice device);
        static bool HasSwapchainSupport(PhysicalDevice device);
    };
}
