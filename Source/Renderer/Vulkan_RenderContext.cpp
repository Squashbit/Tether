#define TETHER_INCLUDE_VULKAN
#include <Tether/Renderer/RenderContext.hpp>
#include <Tether/Renderer/Vulkan/RenderContextNative.hpp>

using namespace Tether::Renderer;

bool RenderContext::CreateVulkanRenderer(VkInstance* pInstance, VkDevice* pDevice)
{
	if (initialized)
		return false;

	native = (IRenderContextNative*)new(std::nothrow) Vulkan::RenderContextNative();
	if (!native)
		return false;

	initialized = true;
	return true;
}