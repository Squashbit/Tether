#include <Tether/Renderer/Vulkan/Instance.hpp>

using namespace Tether::Vulkan;

bool Device::Init(
    Instance* pInstance,
    VkPhysicalDevice physicalDevice,
    VkDeviceQueueCreateInfo* queueFamilies, 
    uint32_t queueFamilyCount, 
    VkPhysicalDeviceFeatures features,
    const char *const * extentions,
    uint32_t extentionCount
)
{
    if (initialized)
        return false;
    
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueFamilies;
    createInfo.queueCreateInfoCount = queueFamilyCount;
    createInfo.pEnabledFeatures = &features;

    createInfo.enabledExtensionCount = extentionCount;
    createInfo.ppEnabledExtensionNames = extentions;

    if (pInstance->IsDebugMode())
    {
        // Enable validation layers

        createInfo.enabledLayerCount   = 
            static_cast<uint32_t>(pInstance->validationLayers.size());
        createInfo.ppEnabledLayerNames = pInstance->validationLayers.data();
    }
    
    // Create the device
    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)
        != VK_SUCCESS)
        return false;
    
    initialized = true;
    return true;
}

VkQueue Device::GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex)
{
    VkQueue queue;
    vkGetDeviceQueue(device, familyIndex, queueIndex, &queue);

    return queue;
}

void Device::WaitIdle()
{
    vkDeviceWaitIdle(device);
}

VkDevice Device::Get()
{
    return device;
}

void Device::OnDispose()
{
    vkDestroyDevice(device, nullptr);
}
