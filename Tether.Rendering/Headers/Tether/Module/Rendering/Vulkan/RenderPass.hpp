#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT RenderPass
	{
	public:
		RenderPass(VkDevice device, DeviceLoader* dloader, 
			VkFormat colorAttachmentFormat);
		~RenderPass();
		TETHER_NO_COPY(RenderPass);

		VkRenderPass Get();
	private:
		VkRenderPass renderPass = nullptr;

		VkDevice device = nullptr;
		DeviceLoader* dloader = nullptr;
	};
}