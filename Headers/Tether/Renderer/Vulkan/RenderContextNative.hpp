// WARNING: 
// THIS IS CURRENTLY A PREVIEW FEATURE.
// IN ORDER TO USE THIS PROPERLY, TETHER MUST BE COMPILED WITH THE 
// TETHER_PREVIEW_FEATURES CMAKE OPTION ENABLED.
//
// THIS IS SUBJECT TO CHANGE, AND MAY NOT RESEMBLE THE FINAL VERSION.

#ifndef _TETHER_RENDERER_VULKAN_RENDERCONTEXTNATIVE_HPP
#define _TETHER_RENDERER_VULKAN_RENDERCONTEXTNATIVE_HPP

#include <Tether/Renderer/RenderContext.hpp>
#include <Tether/Renderer/Vulkan/VulkanContext.hpp>
#include <Tether/Renderer/Vulkan/Common/RenderContextNativeInfo.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
	class RenderContextNative : public Renderer::IRenderContextNative
	{
	public:
		RenderContextNative() = default;
		virtual ~RenderContextNative() {}
		TETHER_NO_COPY(RenderContextNative);

		/**
		 * @brief
		 * Initializes with a Vulkan context. Useful for quickly initializing a
		 * RenderContextNative without having to initialize all Vulkan objects.
		 * See Init(RenderContextNativeInfo*) to specify all objects as custom.
		 * 
		 * @param pContext A pointer to the VulkanContext. The object must stay alive
		 * as long as the native is still in use.
		 */
		bool Init(VulkanContext* pContext);
		/**
		 * @brief
		 * Initializes with all custom Vulkan objects.
		 * 
		 * @param pInfo A pointer to a structure containing the Vulkan objects.
		 */
		bool Init(RenderContextNativeInfo* pInfo);
	private:
		void OnDispose();

		VkInstance instance;
		VkDevice device;
		VkSwapchainKHR swapchain;
	};
}

#endif //_TETHER_RENDERER_VULKAN_RENDERCONTEXTNATIVE_HPP