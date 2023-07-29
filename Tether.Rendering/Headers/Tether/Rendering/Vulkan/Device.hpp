#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Rendering/Vulkan/DeviceLoader.hpp>
#include <Tether/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class Instance;
	class TETHER_EXPORT Device
	{
	public:
		Device(Instance& instance);
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

		QueueFamilyIndices GetQueueFamilyIndices() const;
		
		VkDevice Get() const;
		VkPhysicalDevice GetPhysicalDevice() const;
		const DeviceLoader& GetLoader() const;
	protected:
		void OnDispose();
	private:
		void PickDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtentionSupport(VkPhysicalDevice device,
			const char* const* deviceExtentions, uint64_t extentionCount);
		void FindQueueFamilies(VkPhysicalDevice device);

		Instance& m_Instance;
		const InstanceLoader& m_Iloader;
		
		DeviceLoader m_Loader;

		QueueFamilyIndices m_Indices;

		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_Device = nullptr;
	};
}
