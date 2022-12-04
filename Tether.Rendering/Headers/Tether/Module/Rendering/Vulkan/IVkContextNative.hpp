#pragma once

#include <Tether/Module/Rendering/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>
#include <Tether/Module/Rendering/Vulkan/VertexBuffer.hpp>

#include <vulkan/vulkan.h>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT IVkContextNative
	{
	public:
		virtual VertexBuffer* GetRectangleBuffer() = 0;
		virtual DeviceLoader* GetDeviceLoader() = 0;
	};
}