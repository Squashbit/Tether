#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
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

		QueueFamilyIndices families = m_Instance.FindQueueFamilies(m_PhysicalDevice);

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

	VkDevice Device::Get()
	{
		return m_Device;
	}

	VkPhysicalDevice Device::GetPhysicalDevice()
	{
		return m_PhysicalDevice;
	}

	DeviceLoader& Device::GetLoader()
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

		QueueFamilyIndices families = m_Instance.FindQueueFamilies(device);

		bool extentionsSupported = CheckDeviceExtentionSupport(device,
			deviceExtensions.data(), deviceExtensions.size());

		return
			deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
			&& deviceFeatures.geometryShader
			&& families.hasGraphicsFamily
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
}
