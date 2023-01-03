#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Ref.hpp>

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
    class Instance;
    class TETHER_EXPORT Device : public IDisposable
    {
    public:
        /**
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
        Device(
			Instance* pInstance,
            VkSurfaceKHR surface
        );
        TETHER_NO_COPY(Device);
        
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
        VkPhysicalDevice GetPhysicalDevice();
        DeviceLoader* GetLoader();
    protected:
        void OnDispose();
    private:
        void PickDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtentionSupport(VkPhysicalDevice device,
			const char* const* deviceExtentions, uint64_t extentionCount);

        Instance* instance = nullptr;
        InstanceLoader* iloader = nullptr;
        VkSurfaceKHR surface = nullptr;

        DeviceLoader loader;

        VkPhysicalDevice physicalDevice = nullptr;
        VkDevice device = nullptr;
    };
}
