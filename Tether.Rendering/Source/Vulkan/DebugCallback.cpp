#include <Tether/Module/Rendering/Vulkan/DebugCallback.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>

namespace Tether::Rendering::Vulkan
{
	DebugCallback::~DebugCallback()
	{
		if (m_IsInGlobalVulkan && GlobalVulkan::IsAlive())
			GlobalVulkan::Get().RemoveDebugMessenger(*this);
	}
}