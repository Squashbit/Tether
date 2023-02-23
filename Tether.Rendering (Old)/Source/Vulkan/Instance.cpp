#include <Tether/Native.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <stdexcept>

#include <iostream>
#include <string>
#include <cstring>
#include <set>

#ifdef __linux__
#include <vulkan/vulkan_xlib.h>
#elif _WIN32
#include <vulkan/vulkan_win32.h>
#endif

namespace Tether::Rendering::Vulkan
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		Instance* instance = (Instance*)pUserData;
		instance->DispatchDebugMessage(messageSeverity, messageType,
			pCallbackData);

		return false;
	}

	Instance::Instance(const InstanceInfo& info, bool debugMode)
		:
		m_DebugMode(debugMode),
		EnumerateInstanceLayerProperties(info.EnumerateInstanceLayerProperties)
	{
		// Most structures require an sType that specifies what this structure
		// represents.
		// This struct is needed to tell Vulkan some information about this
		// application.
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;
		appInfo.pApplicationName = info.applicationName;
		appInfo.pEngineName = info.engineName;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> requiredExtentions =
			GetRequiredExtentions(debugMode);

		createInfo.enabledExtensionCount =
			static_cast<uint32_t>(requiredExtentions.size());
		createInfo.ppEnabledExtensionNames = requiredExtentions.data();

		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo{};
		if (debugMode)
		{
			// Initialize validation layers

			if (!CheckValidationLayerSupport(validationLayers))
				throw std::runtime_error("Validation layers not supported");

			createInfo.enabledLayerCount =
				static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			// Initialize debug messenger
			{
				messengerCreateInfo.sType =
					VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				messengerCreateInfo.messageSeverity =
					VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				messengerCreateInfo.messageType =
					VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
					| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				messengerCreateInfo.pfnUserCallback = VulkanDebugCallback;
				messengerCreateInfo.pUserData = this;

				createInfo.pNext = &messengerCreateInfo;
			}
		}

		VkResult result = info.CreateInstance(&createInfo, nullptr, &instance);
		switch (result)
		{
			case VK_SUCCESS: break;

			case VK_ERROR_INCOMPATIBLE_DRIVER:
				throw std::runtime_error("Incompatible Vulkan driver");

			case VK_ERROR_LAYER_NOT_PRESENT:
				throw std::runtime_error("Vulkan validation layers not present");

			default:
				throw std::runtime_error("Failed to create Vulkan instance");
		}

		uint32_t extentionCount = 0;
		info.EnumerateInstanceExtensionProperties(nullptr, &extentionCount, nullptr);

		extentions = std::vector<VkExtensionProperties>(extentionCount);
		info.EnumerateInstanceExtensionProperties(nullptr, &extentionCount,
			extentions.data());

		loader.Load(&instance, info.GetInstanceProcAddr);

		if (debugMode && loader.vkCreateDebugUtilsMessengerEXT)
			loader.vkCreateDebugUtilsMessengerEXT(instance, &messengerCreateInfo,
				nullptr, &debugMessenger);
	}

	Instance::~Instance()
	{
		if (m_DebugMode && loader.vkDestroyDebugUtilsMessengerEXT)
			loader.vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

		debugCallbacks.clear();

		loader.vkDestroyInstance(instance, nullptr);
	}

	QueueFamilyIndices Instance::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices queueFamilies;

		uint32_t familyCount = 0;
		loader.vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

		if (familyCount == 0)
			return queueFamilies;

		std::vector<VkQueueFamilyProperties> families(familyCount);
		loader.vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount,
			families.data());
		
		for (size_t i = 0; i < families.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = families[i];

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT
				&& !queueFamilies.hasGraphicsFamily)
			{
				queueFamilies.hasGraphicsFamily = true;
				queueFamilies.graphicsFamilyIndex = static_cast<uint32_t>(i);
			}

			if (queueFamilies.hasGraphicsFamily)
				break;
		}

		return queueFamilies;
	}

	void Instance::AddDebugMessenger(DebugCallback& callback)
	{
		if (!m_DebugMode)
			return;

		for (uint64_t i = 0; i < debugCallbacks.size(); i++)
			if (debugCallbacks[i] == &callback)
				return;

		debugCallbacks.push_back(&callback);
	}

	void Instance::RemoveDebugMessenger(DebugCallback& callback)
	{
		if (!m_DebugMode)
			return;

		for (uint64_t i = 0; i < debugCallbacks.size(); i++)
			if (debugCallbacks[i] == &callback)
			{
				debugCallbacks.erase(debugCallbacks.begin() + i);
				return;
			}
	}

	VkInstance Instance::Get()
	{
		return instance;
	}

	InstanceLoader& Instance::GetLoader()
	{
		return loader;
	}

	void Instance::DispatchDebugMessage(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
	)
	{
		for (uint64_t i = 0; i < debugCallbacks.size(); i++)
			debugCallbacks[i]->OnDebugLog(messageSeverity, messageType,
				pCallbackData);
	}

	std::vector<VkExtensionProperties> Instance::GetAvailableExtentions()
	{
		return extentions;
	}

	bool Instance::IsDebugMode()
	{
		return m_DebugMode;
	}

	bool Instance::CheckValidationLayerSupport(std::vector<const char*> layers)
	{
		uint32_t layerCount;
		EnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		EnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : layers)
		{
			bool layerFound = false;
			for (const auto& layerProperties : availableLayers)
				if (std::strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}

			if (!layerFound)
				return false;
		}

		return true;
	}

	std::vector<const char*> Instance::GetRequiredExtentions(bool enableValidationLayers)
	{
		std::vector<const char*> extentions;
		extentions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#ifdef _WIN32
		extentions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif __linux__
		extentions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

		if (enableValidationLayers)
			extentions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extentions;
	}
}
