// WARNING: 
// THIS IS CURRENTLY A PREVIEW FEATURE.
// IN ORDER TO USE THIS PROPERLY, TETHER MUST BE COMPILED WITH THE 
// TETHER_PREVIEW_FEATURES CMAKE OPTION ENABLED.
//
// THIS IS SUBJECT TO CHANGE, AND MAY NOT RESEMBLE THE FINAL VERSION.

#ifndef _TETHER_RENDERER_VULKAN_ADVANCEDNATIVE_HPP
#define _TETHER_RENDERER_VULKAN_ADVANCEDNATIVE_HPP

#include <Tether/Renderer/RenderContext.hpp>
#include <Tether/Renderer/Vulkan/Common/AdvancedNativeInfo.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Vulkan
{
	class AdvancedNative : public Renderer::IRenderContextNative
	{
	public:
		AdvancedNative() = default;
		virtual ~AdvancedNative() {}
		TETHER_NO_COPY(AdvancedNative);

		/**
		 * @brief
		 * Initializes the AdvancedNative
		 */
		bool Init();
	private:
		void OnDispose();
	};
}

#endif //_TETHER_RENDERER_VULKAN_ADVANCEDNATIVE_HPP