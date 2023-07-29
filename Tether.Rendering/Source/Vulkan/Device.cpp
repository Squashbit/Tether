#include <Tether/Rendering/Vulkan/Instance.hpp>
#include <stdexcept>

#include <set>

namespace Tether::Rendering::Vulkan
{
	static const std::vector<const char*> deviceExtensions =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	Device::Device(Instance& instance)
		:
		m_Instance(instance),
		m_Iloader(instance.GetLoader())
	{
		PickDevice();

		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo queueCreateInfos[1] = {};
		queueCreateInfos[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfos[0].queueFamilyIndex = families.graphicsFamilyIndex;
		queueCreateInfos[0].queueCount = 1;
		queueCreateInfos[0].pQueuePriorities = &queuePriority;

		VkPhysicalDeviceFeatures features{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = queueCreateInfos;
		createInfo.queueCreateInfoCount = sizeof(queueCreateInfos) 
			/ sizeof(queueCreateInfos[0]);
		createInfo.pEnabledFeatures = &features;
		createInfo.enabledExtensionCount = 
			static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (instance.IsDebugMode())
		{
			// Enable validation layers

			createInfo.enabledLayerCount =
				static_cast<uint32_t>(instance.validationLayers.size());
			createInfo.ppEnabledLayerNames = instance.validationLayers.data();
		}

		// Create the device
		if (m_Iloader.vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device)
			!= VK_SUCCESS)
			throw std::runtime_error("Device creation failed");

		m_Loader.Load(m_Iloader, m_Device);
	}

	Device::~Device()
	{
		m_Loader.vkDestroyDevice(m_Device, nullptr);
	}

	VkQueue Device::GetDeviceQueue(uint32_t familyIndex, uint32_t queueIndex)
	{
		VkQueue queue;
		m_Loader.vkGetDeviceQueue(m_Device, familyIndex, queueIndex, &queue);

		return queue;
	}

	void Device::WaitIdle()
	{
		m_Loader.vkDeviceWaitIdle(m_Device);
	}

	VkPhysicalDeviceProperties Device::GetPhysicalDeviceProperties()
	{
		VkPhysicalDeviceProperties properties{};
		m_Iloader.vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

		return properties;
	}

	VkDevice Device::Get() const
	{
		return m_Device;
	}

	VkPhysicalDevice Device::GetPhysicalDevice() const
{
		return m_PhysicalDevice;
	}

	const DeviceLoader& Device::GetLoader() const
	{
		return m_Loader;
	}

	void Device::PickDevice()
	{
		uint32_t deviceCount = 0;
		m_Iloader.vkEnumeratePhysicalDevices(m_Instance.Get(), &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("No vulkan devices found");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		m_Iloader.vkEnumeratePhysicalDevices(m_Instance.Get(), &deviceCount,
			devices.data());

		for (VkPhysicalDevice device : devices)
			if (IsDeviceSuitable(device))
			{
				m_PhysicalDevice = device;
				return;
			}

		throw std::runtime_error("No suitable device found");
	}

	bool Device::IsDeviceSuitable(VkPhysicalDevice device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		m_Iloader.vkGetPhysicalDeviceProperties(device, &deviceProperties);
		m_Iloader.vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		bool extentionsSupported = CheckDeviceExtentionSupport(device,
			deviceExtensions.data(), deviceExtensions.size());

		return
			deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
			&& deviceFeatures.geometryShader
			&& HasGraphicsFamily(device)
			&& extentionsSupported;
	}

	bool Device::CheckDeviceExtentionSupport(VkPhysicalDevice device,
		const char* const* deviceExtentions, uint64_t extentionCount)
	{
		// The device requires some extentions (such as the swap chain)
		// We need to check for those here before we can use the device.

		uint32_t count;
		m_Iloader.vkEnumerateDeviceExtensionProperties(device, nullptr, &count,
			nullptr);

		std::vector<VkExtensionProperties> availableExtentions(count);
		m_Iloader.vkEnumerateDeviceExtensionProperties(device, nullptr, &count,
			availableExtentions.data());

		std::vector<std::string> requiredExtentions(extentionCount);
		for (uint64_t i = 0; i < extentionCount; i++)
			requiredExtentions[i] = std::string(deviceExtentions[i]);

		std::set<std::string> requiredExtentionSet(requiredExtentions.begin(),
			requiredExtentions.end());

		for (size_t i = 0; i < availableExtentions.size(); i++)
			requiredExtentionSet.erase(availableExtentions[i].extensionName);

		return requiredExtentionSet.empty();
	}

	void Device::FindQueueFamilies(VkPhysicalDevice device)
	{
		// kinda hacky. if this physical device isn't chosen, this function will
		// just run again and overwrite this value, so it will still choose the
		// correct queue family indices. TLDR; it works, it just doesn't look like it does.

		uint32_t familyCount = 0;
		m_Iloader.vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);

		if (familyCount == 0)
			return;

		std::vector<VkQueueFamilyProperties> families(familyCount);
		m_Iloader.vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount,
			families.data());

		for (size_t i = 0; i < families.size(); i++)
		{
			const VkQueueFamilyProperties& queueFamily = families[i];

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT
				&& !m_Indices.hasGraphicsFamily)
			{
				m_Indices.hasGraphicsFamily = true;
				m_Indices.graphicsFamilyIndex = static_cast<uint32_t>(i);
			}

			if (m_Indices.hasGraphicsFamily)
				break;
		}
	}

	QueueFamilyIndices Device::GetQueueFamilyIndices() const
	{
		return m_Indices;
	}
}
