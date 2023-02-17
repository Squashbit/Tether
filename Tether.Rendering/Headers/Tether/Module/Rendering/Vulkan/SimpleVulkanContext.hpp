#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanContext.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>

#include <Tether/SimpleWindow.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT SimpleVulkanContext : public VulkanContext
	{
	public:
		SimpleVulkanContext(SimpleWindow& window);
		~SimpleVulkanContext();
		TETHER_NO_COPY(SimpleVulkanContext);

		VkQueue presentQueue = nullptr;
		Instance& m_Instance;
		Surface surface;
		QueueFamilyIndices queueIndices;
		VkSurfaceFormatKHR surfaceFormat;
		SwapchainDetails swapchainDetails;
	private:
		void CreateRenderPass();
		void CreateCommandPool();

		void QuerySwapchainSupport();
		void ChooseSurfaceFormat();

		Device m_Device;
	};
}