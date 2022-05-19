// WARNING: 
// THIS IS CURRENTLY A PREVIEW FEATURE.
// IN ORDER TO USE THIS PROPERLY, TETHER MUST BE COMPILED WITH THE 
// TETHER_PREVIEW_FEATURES CMAKE OPTION ENABLED.
//
// THIS IS SUBJECT TO CHANGE, AND MAY NOT RESEMBLE THE FINAL VERSION.

#ifndef _TETHER_RENDERER_RENDERCONTEXTNATIVE_HPP
#define _TETHER_RENDERER_RENDERCONTEXTNATIVE_HPP

#include <Tether/Renderer/RenderContext.hpp>
#include <vulkan/vulkan.h>

namespace Tether::Renderer::Vulkan
{
	class RenderContextNative : public IRenderContextNative
	{
	public:
		RenderContextNative() = default;
		virtual ~RenderContextNative() {}
		TETHER_NO_COPY(RenderContextNative);

		bool Init();
	};
}

#endif //_TETHER_RENDERER_RENDERCONTEXTNATIVE_HPP