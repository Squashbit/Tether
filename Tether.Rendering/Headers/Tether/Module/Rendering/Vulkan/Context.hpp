#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/DebugCallback.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>

#include <optional>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	struct VulkanNative;
	class DebugCallback;

	class TETHER_EXPORT Context
	{
	public:
#ifdef _DEBUG
		Context(bool debugMode = true);
#else
		Context(bool debugMode = false);
#endif

		~Context();
		TETHER_NO_COPY(Context);

		// The DebugCallback object must be alive until it is removed.
		void AddDebugMessenger(DebugCallback& debugCallback);
		void RemoveDebugMessenger(DebugCallback& debugCallback);

		PFN_vkGetInstanceProcAddr GetInstanceProcAddr;

		VkInstance instance = nullptr;
		InstanceLoader instanceLoader;
		VkDevice device = nullptr;
		VkQueue queue = nullptr;
		DeviceLoader deviceLoader;
		VkPhysicalDevice physicalDevice = nullptr;
		VmaAllocator allocator = nullptr;
		VkCommandPool commandPool = nullptr;

		QueueFamilyIndices indices;
	private:
		void LoadVulkan();
		void CreateAllocator();
		void CreateCommandPool();

		void* handle = nullptr;

		PFN_vkCreateInstance CreateInstance = nullptr;
		PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;
		PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties = nullptr;
		
		std::optional<Instance> m_Instance;
		std::optional<Device> m_Device;
	};
}
