#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/Surface.hpp>
#include <Tether/Module/Rendering/Vulkan/Device.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/SwapchainDetails.hpp>

#include <vulkan/vulkan.hpp>

namespace Tether::Rendering::Vulkan
{
    class TETHER_EXPORT Swapchain : public IDisposable
    {
    public:
        /**
         * @param pSurface A pointer to the surface object to tie the
         *  swapchain to.
         * @param pDevice A pointer to the device to the the swapchain to.
         * @param createInfo A pointer to a VkSwapchainCreateInfoKHR struct
         *  describing how to set up the swapchain.
         *
         * @returns True if initialization was successful. Otherwise, false.
         */
        Swapchain(
			Surface* pSurface,
			Device* pDevice,
			SwapchainDetails details,
			VkSwapchainCreateInfoKHR* createInfo
        );
        TETHER_NO_COPY(Swapchain);

        /**
         * @brief If vsync is true, this function returns FIFO. 
         *  If vsync is false, mailbox is preferred, but immediate is 
         *  returned if mailbox isn't available; 
         *  if neither are supported, FIFO is returned.
         * 
         * @param presentModes A vector of available present modes.
         * @param vsync If the present mode should be FIFO.
         * 
         * @returns The chosen present mode.
         */
        VkPresentModeKHR ChoosePresentMode(std::vector<VkPresentModeKHR>& presentModes,
            bool vsync);
        /**
         * @brief Chooses an extent with a preferred width and height.
         *  This function returns the width and height clamped within
         *  capabilities.minImageExtent and capabilities.maxImageExtent.
         * 
         * @param capabilities The surface capabilities.
         * @param width The preferred width.
         * @param height The preferred height.
         * 
         * @returns The closest available extent to width and height.
         */
        VkExtent2D ChooseExtent(VkSurfaceCapabilitiesKHR& capabilities,
            uint64_t width, uint64_t height);
        
        uint32_t GetImageCount();
        std::vector<VkImage> GetImages();
        void CreateImageViews(std::vector<VkImageView>* pVec);

        VkExtent2D GetExtent();
        VkFormat GetImageFormat();
        
        VkSwapchainKHR Get();
    protected:
        void OnDispose();
    private:
        uint32_t imageCount = 0;
        VkSwapchainKHR swapchain;

        VkExtent2D extent;
        VkFormat imageFormat;

        Device* pDevice = nullptr;
        DeviceLoader* pLoader = nullptr;
    };
}
