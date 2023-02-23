#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Common/VulkanContext.hpp>

#include <vk_mem_alloc.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT AllocatorManager
	{
	public:
		AllocatorManager(Context& context);
		~AllocatorManager();
		TETHER_NO_COPY(AllocatorManager);
	private:
		Context& m_Context;
	};
}