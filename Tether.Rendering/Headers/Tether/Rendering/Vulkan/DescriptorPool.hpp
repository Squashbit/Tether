#pragma once

#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT DescriptorPool
	{
		friend class DescriptorSet;
	public:
		DescriptorPool(GraphicsContext& context, uint32_t maxSets, uint32_t sizeCount,
			VkDescriptorPoolSize* sizes);
		~DescriptorPool();
		TETHER_NO_COPY(DescriptorPool);

		VkDescriptorPool Get();
	protected:
		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;
	private:
		VkDescriptorPool descriptorPool = nullptr;
	};
}