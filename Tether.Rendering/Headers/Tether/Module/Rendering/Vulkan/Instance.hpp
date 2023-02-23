#pragma once

#include <vector>
#include <unordered_map>

#include <Tether/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/DebugCallback.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>

namespace Tether::Rendering::Vulkan
{
	struct InstanceInfo
	{
		const char* applicationName = "VulkanApp";
		const char* engineName = "VulkanEngine";
		PFN_vkCreateInstance CreateInstance = nullptr;
		PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;
		PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties = nullptr;
		PFN_vkGetInstanceProcAddr GetInstanceProcAddr = nullptr;
	};

	class TETHER_EXPORT Instance
	{
	public:
		Instance(
			const InstanceInfo& info,
			bool debugMode = false
		);
		~Instance();
		TETHER_NO_COPY(Instance);

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		bool CheckDeviceExtentionSupport(VkPhysicalDevice device, 
			const char*const * deviceExtentions, uint64_t extentionCount);
		
		/**
		 * @brief Adds a debug messenger.
		 * 
		 * @param messenger The debug messenger.
		 */
		void AddDebugMessenger(DebugCallback& messenger);
		/**
		 * @brief Removes a debug messenger.
		 * 
		 * @param messenger The debug messenger.
		 */
		void RemoveDebugMessenger(DebugCallback& messenger);

		VkInstance Get();
		InstanceLoader& GetLoader();

		std::vector<VkExtensionProperties> GetAvailableExtentions();

		void DispatchDebugMessage(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
		);

		bool IsDebugMode();

		// Validation layers check when errors happen in Vulkan.
		const std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	private:
		bool CheckValidationLayerSupport(std::vector<const char*> layers);
		std::vector<const char*> GetRequiredExtentions(bool debugMode);

		// The Vulkan instance. Essentially, it is the connection between this
		// application and Vulkan.
		VkInstance instance = nullptr;
		VkDebugUtilsMessengerEXT debugMessenger = nullptr;

		PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties = nullptr;

		Vulkan::InstanceLoader loader;
		
		std::vector<DebugCallback*> debugCallbacks;
		std::vector<VkExtensionProperties> extentions;

		bool m_DebugMode = false;
	};
}
