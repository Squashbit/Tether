#ifndef _TETHER_RENDERER_RENDERCONTEXT_HPP
#define _TETHER_RENDERER_RENDERCONTEXT_HPP

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Defs.hpp>

namespace Tether::Renderer
{
	class RenderContext : public IDisposable
	{
	public:
		RenderContext() {}
		TETHER_NO_COPY(RenderContext);

		/**
		 * @brief Creates a Vulkan component, as in the Vulkan part of the renderer.
		 * 
		 * @param pComponent A pointer to the vulkan component to initialize.
		 * 
		 * @returns True if creation was successful; otherwise, false.
		 */
		bool CreateVulkanComponent(VulkanComponent* pComponent);
	};
}

#endif //_TETHER_RENDERER_RENDERCONTEXT_HPP