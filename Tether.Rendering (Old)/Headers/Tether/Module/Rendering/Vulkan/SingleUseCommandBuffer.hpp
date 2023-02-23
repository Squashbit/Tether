#pragma once

#include <Tether/Module/Rendering/Vulkan/Common/VulkanContext.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT SingleUseCommandBuffer
	{
	public:
		SingleUseCommandBuffer(VulkanContext& context);
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

		VkDevice m_Device = nullptr;
		DeviceLoader& m_Dloader;
		VkCommandPool m_CommandPool = nullptr;
		VkQueue m_Queue = nullptr;
	};
}