#include <Tether/Rendering/Vulkan/DeviceLoader.hpp>
#include <Tether/Application.hpp>

using namespace Tether::Rendering::Vulkan;

#define TETHER_DEVICE_FUNC(name) \
	vk##name = (PFN_vk##name)instanceLoader.vkGetDeviceProcAddr(device, "vk"#name)

void DeviceLoader::Load(InstanceLoader& instanceLoader, VkDevice device)
{
	TETHER_DEVICE_FUNC(AcquireNextImageKHR);
	TETHER_DEVICE_FUNC(AllocateCommandBuffers);
	TETHER_DEVICE_FUNC(AllocateDescriptorSets);
	TETHER_DEVICE_FUNC(BeginCommandBuffer);
	TETHER_DEVICE_FUNC(CreateBuffer);
	TETHER_DEVICE_FUNC(CreateCommandPool);
	TETHER_DEVICE_FUNC(CreateDescriptorPool);
	TETHER_DEVICE_FUNC(CreateDescriptorSetLayout);
	TETHER_DEVICE_FUNC(CreateFence);
	TETHER_DEVICE_FUNC(CreateFramebuffer);
	TETHER_DEVICE_FUNC(CreateGraphicsPipelines);
	TETHER_DEVICE_FUNC(CreateImage);
	TETHER_DEVICE_FUNC(CreateImageView);
	TETHER_DEVICE_FUNC(CreatePipelineLayout);
	TETHER_DEVICE_FUNC(CreateRenderPass);
	TETHER_DEVICE_FUNC(CreateSemaphore);
	TETHER_DEVICE_FUNC(CreateSampler);
	TETHER_DEVICE_FUNC(CreateShaderModule);
	TETHER_DEVICE_FUNC(CreateSwapchainKHR);
	TETHER_DEVICE_FUNC(CmdBeginRenderPass);
	TETHER_DEVICE_FUNC(CmdBindDescriptorSets);
	TETHER_DEVICE_FUNC(CmdBindPipeline);
	TETHER_DEVICE_FUNC(CmdBindIndexBuffer);
	TETHER_DEVICE_FUNC(CmdBindVertexBuffers);
	TETHER_DEVICE_FUNC(CmdCopyBuffer);
	TETHER_DEVICE_FUNC(CmdCopyBufferToImage);
	TETHER_DEVICE_FUNC(CmdDraw);
	TETHER_DEVICE_FUNC(CmdDrawIndexed);
	TETHER_DEVICE_FUNC(CmdPipelineBarrier);
	TETHER_DEVICE_FUNC(CmdPushConstants);
	TETHER_DEVICE_FUNC(CmdEndRenderPass);
	TETHER_DEVICE_FUNC(CmdSetScissor);
	TETHER_DEVICE_FUNC(CmdSetViewport);
	TETHER_DEVICE_FUNC(DestroyBuffer);
	TETHER_DEVICE_FUNC(DestroyCommandPool);
	TETHER_DEVICE_FUNC(DestroyDescriptorPool);
	TETHER_DEVICE_FUNC(DestroyDescriptorSetLayout);
	TETHER_DEVICE_FUNC(DestroyDevice);
	TETHER_DEVICE_FUNC(DestroyImage);
	TETHER_DEVICE_FUNC(DestroyImageView);
	TETHER_DEVICE_FUNC(DestroyFramebuffer);
	TETHER_DEVICE_FUNC(DestroyPipeline);
	TETHER_DEVICE_FUNC(DestroyPipelineLayout);
	TETHER_DEVICE_FUNC(DestroyRenderPass);
	TETHER_DEVICE_FUNC(DestroyFence);
	TETHER_DEVICE_FUNC(DestroySemaphore);
	TETHER_DEVICE_FUNC(DestroySampler);
	TETHER_DEVICE_FUNC(DestroyShaderModule);
	TETHER_DEVICE_FUNC(DestroySwapchainKHR);
	TETHER_DEVICE_FUNC(DeviceWaitIdle);
	TETHER_DEVICE_FUNC(EndCommandBuffer);
	TETHER_DEVICE_FUNC(FreeCommandBuffers);
	TETHER_DEVICE_FUNC(FreeDescriptorSets);
	TETHER_DEVICE_FUNC(GetDeviceQueue);
	TETHER_DEVICE_FUNC(GetSwapchainImagesKHR);
	TETHER_DEVICE_FUNC(QueuePresentKHR);
	TETHER_DEVICE_FUNC(QueueSubmit);
	TETHER_DEVICE_FUNC(QueueWaitIdle);
	TETHER_DEVICE_FUNC(ResetCommandBuffer);
	TETHER_DEVICE_FUNC(ResetFences);
	TETHER_DEVICE_FUNC(UpdateDescriptorSets);
	TETHER_DEVICE_FUNC(WaitForFences);
}
