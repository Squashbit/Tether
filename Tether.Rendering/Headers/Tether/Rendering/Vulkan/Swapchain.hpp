#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Rendering/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Rendering/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>

#include <vulkan/vulkan.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Swapchain
	{
	public:
		Swapchain(
			GraphicsContext& context,
			const SwapchainDetails& details,
			VkSurfaceFormatKHR format,
			VkSurfaceKHR surface,
			uint32_t width, uint32_t height,
			bool vsync
		);
		~Swapchain();
		TETHER_NO_COPY(Swapchain);

		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& presentModes,
			bool vsync);
		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities,
			uint64_t width, uint64_t height);

		std::vector<VkImage> GetImages();
		[[nodiscard]] std::vector<VkImageView> CreateImageViews();

		uint32_t GetImageCount();
		VkExtent2D GetExtent();
		
		VkSwapchainKHR Get();
	private:
		uint32_t FindImageCount(const SwapchainDetails& details);

		VkInstance m_Instance = nullptr;
		const InstanceLoader& m_Iloader;

		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;

		VkSwapchainKHR m_Swapchain = nullptr;

		uint32_t m_ImageCount = 0;
		VkExtent2D m_ImageExtent;
		VkFormat m_ImageFormat = VK_FORMAT_UNDEFINED;
	};
}
