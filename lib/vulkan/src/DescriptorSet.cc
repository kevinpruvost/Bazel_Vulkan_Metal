///
/// Project: Bazel_Vulkan_Metal
/// @file DescriptorSet.cc
/// @date Sep, 16 2024
/// @brief 
/// @author Pruvost Kevin | pruvostkevin (pruvostkevin0@gmail.com)
///
#include <venom/vulkan/DescriptorSet.h>

#include <venom/vulkan/LogicalDevice.h>

namespace venom
{
namespace vulkan
{
DescriptorSet::DescriptorSet()
    : __set(VK_NULL_HANDLE)
{
}

DescriptorSet::~DescriptorSet()
{
    // Automatically freed by the descriptor pool
}

void DescriptorSet::Update(const VkWriteDescriptorSet& write)
{
    vkUpdateDescriptorSets(LogicalDevice::GetVkDevice(), 1, &write, 0, nullptr);
}

void DescriptorSet::Update(UniformBuffer& buffer, uint32_t bufferOffset, uint32_t bufferRange, uint32_t binding,
    VkDescriptorType descriptorType, uint32_t descriptorCount, uint32_t arrayElement)
{
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = buffer.GetVkBuffer();
    bufferInfo.offset = bufferOffset;
    bufferInfo.range = bufferRange;

    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = __set;
    write.dstBinding = binding;
    write.dstArrayElement = arrayElement;
    write.descriptorType = descriptorType;
    write.descriptorCount = descriptorCount;
    write.pBufferInfo = &bufferInfo;

    Update(write);
}

const VkDescriptorSet & DescriptorSet::GetVkDescriptorSet() const
{
    return __set;
}
}
}