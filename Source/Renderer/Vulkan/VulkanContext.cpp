#include <Tether/Renderer/Vulkan/VulkanContext.hpp>

using namespace Tether::Vulkan;

bool VulkanContext::Init(ContextOptions* pOptions)
{
	Application& app = Application::Get();
	if (!app.IsInitialized() && !app.Init())
		return;
	if (!app.IsVulkanLoaded() && !app.LoadVulkan())
		return;

	instance = &app.GetVulkanNative()->instance;
	iloader = instance->GetLoader();

	if (!device.Init(instance, ))
		return false;

	return true;
}

void VulkanContext::OnDispose()
{

}