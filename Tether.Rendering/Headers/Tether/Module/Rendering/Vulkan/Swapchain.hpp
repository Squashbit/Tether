#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/Instance.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>

#include <vulkan/vulkan.hpp>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Swapchain
	{
	public:
		Swapchain(
			Instance* instance,
			Device* device,
			VkSurfaceKHR surface,
			uint32_t width, uint32_t height,
			bool vsync
		);
		~Swapchain();
		TETHER_NO_COPY(Swapchain);

		VkPresentModeKHR ChoosePresentMode(std::vector<VkPresentModeKHR>& presentModes,
			bool vsync);
		VkExtent2D ChooseExtent(VkSurfaceCapabilitiesKHR& capabilities,
			uint64_t width, uint64_t height);

		std::vector<VkImage> GetImages();
		void CreateImageViews(std::vector<VkImageView>* pVec);

		uint32_t GetImageCount();
		VkExtent2D GetExtent();
		VkFormat GetImageFormat();
		
		VkSwapchainKHR Get();
	private:
		SwapchainDetails QuerySwapchainSupport(VkPhysicalDevice physicalDevice, 
			VkSurfaceKHR surface);
		VkSurfaceFormatKHR ChooseSurfaceFormat(SwapchainDetails details);
		uint32_t FindImageCount(SwapchainDetails details);

		Instance* instance = nullptr;
		InstanceLoader* iloader = nullptr;

		Device* device = nullptr;
		DeviceLoader* dloader = nullptr;

		VkSwapchainKHR swapchain = nullptr;

		uint32_t imageCount = 0;
		VkExtent2D imageExtent;
		VkFormat imageFormat = VK_FORMAT_UNDEFINED;
	};
}
