#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT SingleUseCommandBuffer
	{
	public:
		SingleUseCommandBuffer(Device& device, VkCommandPool commandPool,
			VkQueue queue);
		SingleUseCommandBuffer(SingleUseCommandBuffer&& other) noexcept;
		~SingleUseCommandBuffer();
		
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

		void End();

		void Submit(bool wait = true);
		void Wait();
	private:
		bool m_Moved = false;

		VkCommandBuffer m_CommandBuffer = nullptr;
		VkFence m_Fence = nullptr;

		Device& m_Device;
		DeviceLoader* m_Dloader = nullptr;
		VkCommandPool m_CommandPool = nullptr;
		VkQueue m_Queue = nullptr;
	};
}