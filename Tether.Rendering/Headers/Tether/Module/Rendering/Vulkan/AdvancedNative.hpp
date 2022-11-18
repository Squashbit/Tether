#pragma once

#include <Tether/Module/Rendering/RenderContext.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/AdvancedNativeInfo.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT AdvancedNative : public Rendering::IRenderContextNative
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
