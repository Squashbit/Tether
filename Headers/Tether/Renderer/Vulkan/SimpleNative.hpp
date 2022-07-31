// WARNING: 
// THIS IS CURRENTLY A PREVIEW FEATURE.
// IN ORDER TO USE THIS PROPERLY, TETHER MUST BE COMPILED WITH THE 
// TETHER_PREVIEW_FEATURES CMAKE OPTION ENABLED.
//
// THIS IS SUBJECT TO CHANGE, AND MAY NOT RESEMBLE THE FINAL VERSION.

// Useful for quickly creating a Render Context Native without having to initialize 
// all Vulkan objects manually. 
// See AdvancedNative to manually specify Vulkan objects.

#ifndef _TETHER_RENDERER_VULKAN_SIMPLENATIVE_HPP
#define _TETHER_RENDERER_VULKAN_SIMPLENATIVE_HPP

#include <Tether/Renderer/Vulkan/Common/ErrorCode.hpp>
#include <Tether/Renderer/Vulkan/Common/QueueFamilyIndices.hpp>
#include <Tether/Renderer/Vulkan/Common/SwapchainDetails.hpp>
#include <Tether/Renderer/Vulkan/Instance.hpp>
#include <Tether/Renderer/Vulkan/Surface.hpp>
#include <Tether/Renderer/Vulkan/Device.hpp>
#include <Tether/Renderer/Vulkan/Swapchain.hpp>
#include <Tether/Renderer/RenderContext.hpp>

#include <vector>

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
	class SimpleNative : public Renderer::IRenderContextNative
	{
	public:
		SimpleNative() = default;
		virtual ~SimpleNative() {}
		TETHER_NO_COPY(SimpleNative);

		/**
		 * @brief
		 * Initializes the SimpleNative. 
		 */
		ErrorCode Init(SimpleWindow* pWindow);
	private:
		bool InitDevice();
		bool InitSwapchain();
		bool InitRenderPass();

		VkSurfaceFormatKHR ChooseSurfaceFormat(SwapchainDetails details);

		void OnDispose();

		SimpleWindow* pWindow = nullptr;

		InstanceLoader* iloader = nullptr;
		Instance* instance = nullptr;

		DeviceLoader* dloader = nullptr;
		Device device;

		Surface surface;
		Swapchain swapchain;

		Vulkan::QueueFamilyIndices queueIndices;
		VkPhysicalDevice physicalDevice;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkRenderPass renderPass;

		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
	};
}

#endif //_TETHER_RENDERER_VULKAN_SIMPLENATIVE_HPP