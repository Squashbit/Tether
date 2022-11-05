#include <Tether/Module/Rendering/Vulkan/DeviceLoader.hpp>
#include <Tether/Application.hpp>

#define TETHER_INCLUDE_VULKAN
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>

using namespace Tether::Rendering::Vulkan;

void DeviceLoader::Load(InstanceLoader* iloader, VkDevice* pDevice)
{
	if (!iloader || !pDevice)
		return;

	TETHER_DEVICE_FUNC(AcquireNextImageKHR);
	TETHER_DEVICE_FUNC(AllocateCommandBuffers);
	TETHER_DEVICE_FUNC(BeginCommandBuffer);
	TETHER_DEVICE_FUNC(CreateBuffer);
	TETHER_DEVICE_FUNC(CreateCommandPool);
	TETHER_DEVICE_FUNC(CreateFence);
	TETHER_DEVICE_FUNC(CreateFramebuffer);
	TETHER_DEVICE_FUNC(CreateGraphicsPipelines);
	TETHER_DEVICE_FUNC(CreateImageView);
	TETHER_DEVICE_FUNC(CreatePipelineLayout);
	TETHER_DEVICE_FUNC(CreateRenderPass);
	TETHER_DEVICE_FUNC(CreateSemaphore);
	TETHER_DEVICE_FUNC(CreateShaderModule);
	TETHER_DEVICE_FUNC(CreateSwapchainKHR);
	TETHER_DEVICE_FUNC(CmdBeginRenderPass);
	TETHER_DEVICE_FUNC(CmdBindPipeline);
	TETHER_DEVICE_FUNC(CmdBindIndexBuffer);
	TETHER_DEVICE_FUNC(CmdBindVertexBuffers);
	TETHER_DEVICE_FUNC(CmdCopyBuffer);
	TETHER_DEVICE_FUNC(CmdDraw);
	TETHER_DEVICE_FUNC(CmdDrawIndexed);
	TETHER_DEVICE_FUNC(CmdEndRenderPass);
	TETHER_DEVICE_FUNC(CmdSetScissor);
	TETHER_DEVICE_FUNC(CmdSetViewport);
	TETHER_DEVICE_FUNC(DestroyBuffer);
	TETHER_DEVICE_FUNC(DestroyCommandPool);
	TETHER_DEVICE_FUNC(DestroyDevice);
	TETHER_DEVICE_FUNC(DestroyImageView);
	TETHER_DEVICE_FUNC(DestroyFramebuffer);
	TETHER_DEVICE_FUNC(DestroyPipeline);
	TETHER_DEVICE_FUNC(DestroyPipelineLayout);
	TETHER_DEVICE_FUNC(DestroyRenderPass);
	TETHER_DEVICE_FUNC(DestroyFence);
	TETHER_DEVICE_FUNC(DestroySemaphore);
	TETHER_DEVICE_FUNC(DestroyShaderModule);
	TETHER_DEVICE_FUNC(DestroySwapchainKHR);
	TETHER_DEVICE_FUNC(DeviceWaitIdle);
	TETHER_DEVICE_FUNC(EndCommandBuffer);
	TETHER_DEVICE_FUNC(FreeCommandBuffers);
	TETHER_DEVICE_FUNC(GetDeviceQueue);
	TETHER_DEVICE_FUNC(GetSwapchainImagesKHR);
	TETHER_DEVICE_FUNC(ResetCommandBuffer);
	TETHER_DEVICE_FUNC(ResetFences);
	TETHER_DEVICE_FUNC(QueuePresentKHR);
	TETHER_DEVICE_FUNC(QueueSubmit);
	TETHER_DEVICE_FUNC(WaitForFences);
}
