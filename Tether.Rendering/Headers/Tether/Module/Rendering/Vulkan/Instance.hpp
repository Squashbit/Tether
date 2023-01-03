#pragma once

#include <vector>
#include <unordered_map>

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/DebugCallback.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Instance : public IDisposable
	{
	public:
		Instance() {}
		TETHER_NO_COPY(Instance);

		/**
		 * @brief Initializes Vulkan.
		 * 
		 * @param applicationName The name of the application.
		 * @param engineName The name of the engine.
		 * @param debugMode True if the Vulkan instance should be in
		 *  debug mode.
		 * 
		 * @returns True if initialization succeeded. Otherwise, false.
		 */
		VkResult Init(
			const char* applicationName = "VulkanApp",
			const char* engineName = "VulkanEngine",
			bool debugMode = false
		);

		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device,
			VkSurfaceKHR surface);
		SwapchainDetails QuerySwapchainSupport(VkPhysicalDevice device, 
			VkSurfaceKHR surface);
		bool CheckDeviceExtentionSupport(VkPhysicalDevice device, 
			const char*const * deviceExtentions, uint64_t extentionCount);
		
		/**
		 * @brief Adds a debug messenger.
		 * 
		 * @param messenger The debug messenger.
		 */
		void AddDebugMessenger(DebugCallback* messenger);
		/**
		 * @brief Removes a debug messenger.
		 * 
		 * @param messenger The debug messenger.
		 */
		void RemoveDebugMessenger(DebugCallback* messenger);

		VkInstance Get();
		Vulkan::InstanceLoader* GetLoader();

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
	protected:
		void OnDispose();
	private:
		bool CheckValidationLayerSupport(std::vector<const char*> layers);
		std::vector<const char*> GetRequiredExtentions(bool debugMode);

		// The Vulkan instance. Essentially, it is the connection between this
		// application and Vulkan.
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;

		Vulkan::InstanceLoader loader;
		
		std::vector<DebugCallback*> debugCallbacks;
		std::vector<VkExtensionProperties> extentions;

		bool debugMode = false;
	};
}
