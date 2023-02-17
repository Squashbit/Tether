#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class Instance;
	class TETHER_EXPORT Device
	{
	public:
		Device(Instance& instance, VkSurfaceKHR surface);
		~Device();
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

		VkPhysicalDeviceProperties GetPhysicalDeviceProperties();
		
		VkDevice Get();
		VkPhysicalDevice GetPhysicalDevice();
		DeviceLoader& GetLoader();
	protected:
		void OnDispose();
	private:
		void PickDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtentionSupport(VkPhysicalDevice device,
			const char* const* deviceExtentions, uint64_t extentionCount);

		Instance& m_Instance;
		InstanceLoader& m_Iloader;
		VkSurfaceKHR m_Surface = nullptr;

		DeviceLoader m_Loader;

		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_Device = nullptr;
	};
}
