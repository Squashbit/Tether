#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT SingleUseCommandBuffer
	{
	public:
		SingleUseCommandBuffer(Device* pDevice, VkCommandPool commandPool,
			VkQueue queue);
		~SingleUseCommandBuffer();
		TETHER_NO_COPY(SingleUseCommandBuffer);

		// Calls vkBeginCommandBuffer and returns the command buffer created in
		// the constructor.
		VkCommandBuffer Begin();

		void CopyBufferToImage(
			VkBuffer buffer, VkImage image,
			uint32_t width, uint32_t height
		);

		void TransitionImageLayout(
			VkImage image, VkFormat format,
			VkImageLayout oldLayout, VkImageLayout newLayout
		);

		void Submit();
	private:
		VkCommandBuffer commandBuffer = nullptr;

		Device* pDevice = nullptr;
		DeviceLoader* dloader = nullptr;
		VkCommandPool commandPool = nullptr;
		VkQueue queue = nullptr;
	};
}