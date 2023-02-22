#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/VulkanContext.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>

#include <Tether/Window.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT SimpleVulkanContext : public VulkanContext
	{
	public:
		SimpleVulkanContext();
		~SimpleVulkanContext();
		TETHER_NO_COPY(SimpleVulkanContext);

		
		QueueFamilyIndices queueIndices;
	private:
		void CreateCommandPool();

		Instance& m_Instance;
		Device m_Device;
	};
}