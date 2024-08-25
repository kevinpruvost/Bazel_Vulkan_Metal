///
/// Project: VenomEngine
/// @file Fence.cc
/// @date Aug, 23 2024
/// @brief
/// @author Pruvost Kevin | pruvostkevin (pruvostkevin0@gmail.com)
///
#include <venom/vulkan/Fence.h>

namespace venom::vulkan
{
Fence::Fence()
    : __fence(VK_NULL_HANDLE)
    , __logicalDevice(VK_NULL_HANDLE)
{
}

Fence::~Fence()
{
    if (__fence != VK_NULL_HANDLE) {
        vkDestroyFence(__logicalDevice, __fence, nullptr);
    }
}

Fence::Fence(Fence&& other)
    : __fence(other.__fence)
    , __logicalDevice(other.__logicalDevice)
{
    other.__fence = VK_NULL_HANDLE;
    other.__logicalDevice = VK_NULL_HANDLE;
}

Fence& Fence::operator=(Fence&& other)
{
    if (this == &other) return *this;
    __fence = other.__fence;
    __logicalDevice = other.__logicalDevice;
    other.__fence = VK_NULL_HANDLE;
    other.__logicalDevice = VK_NULL_HANDLE;
    return *this;
}

vc::Error Fence::InitFence(const VkDevice logicalDevice, const VkFenceCreateFlags flags)
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = flags;

    if (vkCreateFence(logicalDevice, &fenceCreateInfo, nullptr, &__fence) != VK_SUCCESS) {
        vc::Log::Error("Failed to create fence");
        return vc::Error::Failure;
    }
    __logicalDevice = logicalDevice;
    return vc::Error::Success;
}

const VkFence * Fence::GetFence() const
{
    return &__fence;
}
}
