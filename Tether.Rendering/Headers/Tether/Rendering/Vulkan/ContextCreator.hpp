#pragma once

#include <Tether/Common/Library.hpp>

#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <Tether/Rendering/Vulkan/Instance.hpp>
#include <Tether/Rendering/Vulkan/Device.hpp>

#include <vk_mem_alloc.h>
#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ContextCreator
	{
	public:
		class VulkanLibrary : public Library
		{
		public:
			VulkanLibrary();

			TETHER_VULKAN_FUNC_VAR(GetInstanceProcAddr);
			TETHER_VULKAN_FUNC_VAR(CreateInstance);
			TETHER_VULKAN_FUNC_VAR(EnumerateInstanceExtensionProperties);
			TETHER_VULKAN_FUNC_VAR(EnumerateInstanceLayerProperties);
		};

#ifdef NDEBUG
		ContextCreator(
			bool debugMode = false,
			std::string_view applicationName = "VulkanApp",
			std::string_view engineName = "Tether"
		);
#else
		ContextCreator(
			bool debugMode = true, 
			std::string_view applicationName = "VulkanApp",
			std::string_view engineName = "Tether"
		);
#endif
		~ContextCreator();
		TETHER_NO_COPY(ContextCreator);

		void AddDebugMessenger(DebugCallback* debugCallback);
		void RemoveDebugMessenger(DebugCallback* debugCallback);

		const uint32_t GetFramesInFlight() const;
		const InstanceLoader& GetInstanceLoader() const;
		const DeviceLoader& GetDeviceLoader() const;
		const VulkanLibrary& GetVulkanLibrary() const;
		VkInstance GetInstance() const;
		VkDevice GetDevice() const;
		VkQueue GetQueue() const;
		VkPhysicalDevice GetPhysicalDevice() const;
		VkCommandPool GetCommandPool() const;
	private:
		void CreateCommandPool(uint32_t graphicsFamilyIndex);

		VulkanLibrary m_VulkanLibrary;

		const uint32_t m_FramesInFlight = 2;

		std::optional<Instance> m_Instance;
		std::optional<Device> m_Device;

		VkQueue m_Queue = nullptr;

		VkCommandPool m_CommandPool = nullptr;
	};
}