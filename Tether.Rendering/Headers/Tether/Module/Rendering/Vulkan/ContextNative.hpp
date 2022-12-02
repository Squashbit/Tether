#pragma once

#include <Tether/Module/Rendering/UIRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/AdvancedNativeInfo.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT ContextNative : public Rendering::UIRendererNative
	{
	public:
		ContextNative() = default;
		virtual ~ContextNative() {}
		TETHER_NO_COPY(ContextNative);
	private:
		void OnDispose();
	};
}
