// This file includes window utilities that are platform specific.

#ifndef _TETHER_RENDERER_RENDERCONTEXTNATIVE_HPP
#define _TETHER_RENDERER_RENDERCONTEXTNATIVE_HPP

#include <Tether/Common/Defs.hpp>
#ifdef TETHER_INCLUDE_VULKAN

#include <Tether/Renderer/RenderContext.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Renderer::Vulkan
{
	class RenderContextNative : public IRenderContextNative
	{
	public:
		virtual ~RenderContextNative() {}
	};
}

#endif // TETHER_INCLUDE_VULKAN
#endif //_TETHER_RENDERER_RENDERCONTEXTNATIVE_HPP