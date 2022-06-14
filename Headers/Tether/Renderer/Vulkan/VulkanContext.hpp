#ifndef _TETHER_VULKAN_VULKANCONTEXT_HPP
#define _TETHER_VULKAN_VULKANCONTEXT_HPP

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

#include <Tether/Renderer/Vulkan/InstanceLoader.hpp>
#include <Tether/Renderer/Vulkan/DeviceLoader.hpp>
#include <Tether/Renderer/Vulkan/Instance.hpp>
#include <Tether/Renderer/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
	struct ContextOptions
	{

	};

	class VulkanContext : public IDisposable
	{
	public:
		VulkanContext() = default;
		TETHER_DISPOSE_ON_DESTRUCT(VulkanContext);
		TETHER_NO_COPY(VulkanContext);

		bool Init(ContextOptions* pOptions);

		VkDevice vkDevice;
		VkSwapchainKHR vkSwapchain;

		InstanceLoader* iloader;
		Instance* instance;
		Device device;
	private:
		void OnDispose();
	};
}

#endif //_TETHER_VULKAN_VULKANCONTEXT_HPP