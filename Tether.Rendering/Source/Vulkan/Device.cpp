#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

using namespace Tether::Rendering::Vulkan;

Device::Device(
    Instance* pInstance,
    VkPhysicalDevice physicalDevice,
    VkDeviceQueueCreateInfo* queueFamilies, 
    uint32_t queueFamilyCount, 
    VkPhysicalDeviceFeatures features,
    const char *const * extentions,
    uint32_t extentionCount
)
{
    TETHER_ASSERT(pInstance != nullptr);
    
    this->pInstance = pInstance;
    this->iloader = pInstance->GetLoader();
    
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
    if (iloader->vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)
        != VK_SUCCESS)
        throw RendererException("Device creation failed");

    loader.Load(iloader, &device);
    
    initialized = true;
}

VkQueue Device::GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex)
{
    VkQueue queue;
    loader.vkGetDeviceQueue(device, familyIndex, queueIndex, &queue);

    return queue;
}

void Device::WaitIdle()
{
    loader.vkDeviceWaitIdle(device);
}

VkDevice Device::Get()
{
    return device;
}

DeviceLoader* Device::GetLoader()
{
	return &loader;
}

void Device::OnDispose()
{
    loader.vkDestroyDevice(device, nullptr);
}
