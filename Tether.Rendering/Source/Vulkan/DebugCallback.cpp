#include <Tether/Rendering/Vulkan/DebugCallback.hpp>
namespace Tether::Rendering::Vulkan
{
	DebugCallback::~DebugCallback()
	{
		if (m_IsInGlobalVulkan && GlobalVulkan::IsAlive())
			GlobalVulkan::Get().RemoveDebugMessenger(*this);
	}
}