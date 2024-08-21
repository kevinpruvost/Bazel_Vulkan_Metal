///
/// Project: Bazel_Vulkan_Metal
/// File: VulkanSwapChain.cc
/// Date: 8/21/2024
/// Description: 
/// Author: Pruvost Kevin | pruvostkevin (pruvostkevin0@gmail.com)
///
#include "VulkanSwapChain.h"

#include <algorithm>

namespace venom
{

VulkanSwapChain::VulkanSwapChain()
    : capabilities()
    , surfaceFormats()
    , presentModes()
    , activeSurfaceFormat(VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
    , activePresentMode(VK_PRESENT_MODE_MAX_ENUM_KHR)
    , extent(0, 0)
    , swapChain(VK_NULL_HANDLE)
{
}

VulkanSwapChain::~VulkanSwapChain()
{
    if (swapChain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(__logicalDevice, swapChain, nullptr);
    }
}

Error VulkanSwapChain::InitSwapChainSettings(const VulkanPhysicalDevice* physicalDevice, const VulkanSurface* surface,
    const Context* context, const MappedVulkanQueueFamilies* queueFamilies)
{
        // Get surface capabilities
    if (auto err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->physicalDevice, surface->surface, &capabilities); err != VK_SUCCESS)
    {
        Log::Error("Failed to get physical device surface capabilities: %d", err);
        return Error::InitializationFailed;
    }

    // Get surface formats
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->physicalDevice, surface->surface, &formatCount, nullptr);

    if (formatCount != 0) {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice->physicalDevice, surface->surface, &formatCount, surfaceFormats.data());
    }

    // Get present modes
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->physicalDevice, surface->surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice->physicalDevice, surface->surface, &presentModeCount, presentModes.data());
    }

    // Default active settings
    // SRGB format because it's the most discernable to the human eye
    for (const auto& format : surfaceFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            activeSurfaceFormat = format;
            break;
        }
    }
    // Triple Buffering (Mailbox) if possible, otherwise Double Buffering (FIFO), Single Buffering is Immediate
    for (const auto& presentMode : presentModes) {
        if (presentMode == VK_PRESENT_MODE_FIFO_KHR)
            activePresentMode = presentMode;
        else if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            activePresentMode = presentMode;
            break;
        }
    }
    // Extent, if currentExtent is UINT32_MAX, then the extent can vary, else it's the currentExtent
    if (capabilities.currentExtent.width != UINT32_MAX) {
        extent = capabilities.currentExtent;
    } else {
        // Gets the window size in terms of total pixels, not to confuse with screen coordinates
        // Otherwise we would be using glfwGetWindowSize
        int w, h;
        glfwGetFramebufferSize(const_cast<GLFWwindow*>(context->GetWindow()), &w, &h);

        extent.width = std::clamp<uint32_t>(w, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        extent.height = std::clamp<uint32_t>(h, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }
    return Error::Success;
}

Error VulkanSwapChain::InitSwapChain(const VulkanPhysicalDevice* physicalDevice, const VulkanSurface * surface,
                                     const Context * context, const MappedVulkanQueueFamilies * queueFamilies)
{
    venom_assert(capabilities.maxImageCount > 0, "Swap chain must have at least 1 image");
    // Image count, must be at least the minimum image count, but no more than the maximum image count
    // One more is recommeneded to avoid waiting on the driver
    uint32_t imageCount = std::clamp(capabilities.minImageCount + 1, capabilities.minImageCount, capabilities.maxImageCount);

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = activeSurfaceFormat.format;
    createInfo.imageColorSpace = activeSurfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1; // Always 1 unless stereoscopic 3D
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (queueFamilies->graphicsQueueFamilyIndices[0] != queueFamilies->presentQueueFamilyIndices[0]) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilies->graphicsQueueFamilyIndices.data();
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR == 0) {
        Log::Error("Opaque composite alpha not supported");
        return Error::InitializationFailed;
    }

    createInfo.presentMode = activePresentMode;

    // Clip pixels that are obscured by other windows
    createInfo.clipped = VK_TRUE;

    // Old swap chain, if it exists, VK_NULL_HANDLE on the first try
    createInfo.oldSwapchain = swapChain;

    if (vkCreateSwapchainKHR(physicalDevice->logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
        Log::Error("Failed to create swap chain");
        return Error::InitializationFailed;
    }
    __logicalDevice = physicalDevice->logicalDevice;
    return Error::Success;
}
}
