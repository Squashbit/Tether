#include <Tether/Module/Rendering/Vulkan/ObjectNatives/VkObjectNative.hpp>

using namespace Tether::Rendering::Vulkan::Natives;

VkObjectNative::VkObjectNative(IVkContextNative* pContextNative)
	:
	pContextNative(pContextNative)
{

}