#include <Tether/Module/Rendering/Vulkan/DebugCallback.hpp>
#include <Tether/Module/Rendering/Vulkan/Context.hpp>

namespace Tether::Rendering::Vulkan
{
	DebugCallback::DebugCallback(Context& context)
		:
		m_Context(context)
	{}

	DebugCallback::~DebugCallback()
	{
		if (m_IsInContext)
			m_Context.RemoveDebugMessenger(*this);
	}
}