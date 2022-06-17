#include <Tether/Renderer/Vulkan/RenderContextNative.hpp>

using namespace Tether::Vulkan;

bool RenderContextNative::Init(VulkanContext* pContext)
{
	instance = pContext->instance->Get();
	device = pContext->device.Get();
	//swapchain = pContext->swapchain.Get();

	return true;
}

bool RenderContextNative::Init(RenderContextNativeInfo* pInfo)
{
	instance = pInfo->instance;
	device = pInfo->device;
	swapchain = pInfo->swapchain;

	return true;
}

void RenderContextNative::OnDispose()
{
	
}