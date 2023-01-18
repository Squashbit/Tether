#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Objects/Object.hpp>
#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ObjectRenderer : public Objects::ObjectRenderer
	{
	public:
		virtual void AddToCommandBuffer(VkCommandBuffer commandBuffer,
			uint32_t index) {}
	};
}