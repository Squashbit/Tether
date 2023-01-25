#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT DescriptorPool
	{
	public:
		DescriptorPool(Device* pDevice, uint32_t maxSets, uint32_t sizeCount,
			VkDescriptorPoolSize* sizes);
		~DescriptorPool();
		TETHER_NO_COPY(DescriptorPool);

		VkDescriptorPool Get();
	private:
		Device* pDevice = nullptr;
		DeviceLoader* dloader = nullptr;

		VkDescriptorPool descriptorPool = nullptr;
	};
}