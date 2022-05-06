#ifndef _TETHER_RENDERER_VULKANCOMPONENT_HPP
#define _TETHER_RENDERER_VULKANCOMPONENT_HPP

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Defs.hpp>

namespace Tether::Renderer
{
	class VulkanComponent : public IDisposable
	{
	public:
		VulkanComponent() {}
		TETHER_NO_COPY(VulkanComponent);
	};
}

#endif //_TETHER_RENDERER_VULKANCOMPONENT_HPP