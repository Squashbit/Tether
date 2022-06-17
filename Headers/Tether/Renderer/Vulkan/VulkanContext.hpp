#ifndef _TETHER_VULKAN_VULKANCONTEXT_HPP
#define _TETHER_VULKAN_VULKANCONTEXT_HPP

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <Tether/Renderer/Vulkan/InstanceLoader.hpp>
#include <Tether/Renderer/Vulkan/DeviceLoader.hpp>
#include <Tether/Renderer/Vulkan/Instance.hpp>
#include <Tether/Renderer/Vulkan/Surface.hpp>
#include <Tether/Renderer/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
	struct ContextOptions
	{
		Tether::SimpleWindow* pWindow = nullptr;
	};

	class VulkanContext : public IDisposable
	{
	public:
		enum class ErrorCode
		{
			SUCCESS,
			APP_INIT_FAILED,
			SURFACE_INIT_FAILED,
			DEVICE_NOT_FOUND,
			DEVICE_INIT_FAILED
		};

		VulkanContext() = default;
		TETHER_DISPOSE_ON_DESTRUCT(VulkanContext);
		TETHER_NO_COPY(VulkanContext);

		ErrorCode Init(ContextOptions* pOptions);

		InstanceLoader* iloader;
		Instance* instance;

		Surface surface;
		Device device;

		Vulkan::QueueFamilyIndices queueIndices;
		VkPhysicalDevice physicalDevice;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
	private:
		bool InitDevice();

		void OnDispose();
	};
}

#endif //_TETHER_VULKAN_VULKANCONTEXT_HPP