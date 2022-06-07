#ifndef _TETHER_VULKAN_DEVICE_HPP
#define _TETHER_VULKAN_DEVICE_HPP

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <Tether/Renderer/Vulkan/InstanceLoader.hpp>
#include <Tether/Renderer/Vulkan/DeviceLoader.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
    class Instance;
    class Device : public IDisposable
    {
    public:
        Device() {}
        TETHER_NO_COPY(Device);
        
        /**
         * @brief 
         * Initializes the device.
         * 
         * @param pInstance A pointer to the Vulkan instance.
         * @param physicalDevice The physical device.
         * @param queueFamilies An array of VkDeviceQueueCreateInfo specifying
         *  the queue families that this device should use and describing them.
         * @param queueFamilyCount The number of VkDeviceQueueCreateInfo
         *  in the array.
         * @param features The features that this device should use.
         * @param extentions An array of extentions for this device to use.
         * @param extentionCount The number of extentions.
         * 
         * @returns True if initialization succeeded; otherwise, false.
         */
        bool Init(
            Instance* pInstance,
            VkPhysicalDevice physicalDevice,
            VkDeviceQueueCreateInfo* queueFamilies, 
            uint32_t queueFamilyCount,
            VkPhysicalDeviceFeatures features,
            const char *const * extentions = nullptr,
            uint32_t extentionCount = 0
        );

        /**
         * @brief Get the device queue at the specified index.
         * 
         * @param familyIndex The queue family.
         * @param queueIndex The index of the queue in the family.
         * 
         * @returns The queue object.
         */
        VkQueue GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex);

        /**
         * @brief Waits for all work on the device to finish.
         */
        void WaitIdle();
        
        VkDevice Get();
        DeviceLoader* GetLoader();
    protected:
        void OnDispose();
    private:
        Instance* pInstance = nullptr;
        InstanceLoader* iloader = nullptr;

        DeviceLoader loader;

        VkDevice device;
    };
}

#endif //_TETHER_VULKAN_DEVICE_HPP
