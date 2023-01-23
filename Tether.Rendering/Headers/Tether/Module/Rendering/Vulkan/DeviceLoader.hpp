#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/InstanceLoader.hpp>

#include <vulkan/vulkan.h>

#if defined(_WIN32) || defined(CreateSemaphore)
#undef CreateSemaphore
#endif

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT DeviceLoader
	{
	public:
		DeviceLoader() = default;
		
		void Load(InstanceLoader* pInstLoader, VkDevice* pDevice);

		TETHER_DEVICE_FUNC_VAR(AcquireNextImageKHR);
		TETHER_DEVICE_FUNC_VAR(AllocateCommandBuffers);
		TETHER_DEVICE_FUNC_VAR(AllocateDescriptorSets);
		TETHER_DEVICE_FUNC_VAR(BeginCommandBuffer);
		TETHER_DEVICE_FUNC_VAR(CreateBuffer);
		TETHER_DEVICE_FUNC_VAR(CreateCommandPool);
		TETHER_DEVICE_FUNC_VAR(CreateDescriptorPool);
		TETHER_DEVICE_FUNC_VAR(CreateDescriptorSetLayout);
		TETHER_DEVICE_FUNC_VAR(CreateFence);
		TETHER_DEVICE_FUNC_VAR(CreateFramebuffer);
		TETHER_DEVICE_FUNC_VAR(CreateGraphicsPipelines);
		TETHER_DEVICE_FUNC_VAR(CreateImage);
		TETHER_DEVICE_FUNC_VAR(CreateImageView);
		TETHER_DEVICE_FUNC_VAR(CreatePipelineLayout);
		TETHER_DEVICE_FUNC_VAR(CreateRenderPass);
		TETHER_DEVICE_FUNC_VAR(CreateSampler);
		TETHER_DEVICE_FUNC_VAR(CreateSemaphore);
		TETHER_DEVICE_FUNC_VAR(CreateShaderModule);
		TETHER_DEVICE_FUNC_VAR(CreateSwapchainKHR);
		TETHER_DEVICE_FUNC_VAR(CmdBeginRenderPass);
		TETHER_DEVICE_FUNC_VAR(CmdBindDescriptorSets);
		TETHER_DEVICE_FUNC_VAR(CmdBindPipeline);
		TETHER_DEVICE_FUNC_VAR(CmdBindIndexBuffer);
		TETHER_DEVICE_FUNC_VAR(CmdBindVertexBuffers);
		TETHER_DEVICE_FUNC_VAR(CmdCopyBuffer);
		TETHER_DEVICE_FUNC_VAR(CmdCopyBufferToImage);
		TETHER_DEVICE_FUNC_VAR(CmdDraw);
		TETHER_DEVICE_FUNC_VAR(CmdDrawIndexed);
		TETHER_DEVICE_FUNC_VAR(CmdPipelineBarrier);
		TETHER_DEVICE_FUNC_VAR(CmdEndRenderPass);
		TETHER_DEVICE_FUNC_VAR(CmdSetScissor);
		TETHER_DEVICE_FUNC_VAR(CmdSetViewport);
		TETHER_DEVICE_FUNC_VAR(DestroyBuffer);
		TETHER_DEVICE_FUNC_VAR(DestroyCommandPool);
		TETHER_DEVICE_FUNC_VAR(DestroyDescriptorPool);
		TETHER_DEVICE_FUNC_VAR(DestroyDescriptorSetLayout);
		TETHER_DEVICE_FUNC_VAR(DestroyDevice);
		TETHER_DEVICE_FUNC_VAR(DestroyImage);
		TETHER_DEVICE_FUNC_VAR(DestroyImageView);
		TETHER_DEVICE_FUNC_VAR(DestroyFramebuffer);
		TETHER_DEVICE_FUNC_VAR(DestroyPipeline);
		TETHER_DEVICE_FUNC_VAR(DestroyPipelineLayout);
		TETHER_DEVICE_FUNC_VAR(DestroyRenderPass);
		TETHER_DEVICE_FUNC_VAR(DestroyFence);
		TETHER_DEVICE_FUNC_VAR(DestroySemaphore);
		TETHER_DEVICE_FUNC_VAR(DestroySampler);
		TETHER_DEVICE_FUNC_VAR(DestroyShaderModule);
		TETHER_DEVICE_FUNC_VAR(DestroySwapchainKHR);
		TETHER_DEVICE_FUNC_VAR(DeviceWaitIdle);
		TETHER_DEVICE_FUNC_VAR(EndCommandBuffer);
		TETHER_DEVICE_FUNC_VAR(FreeCommandBuffers);
		TETHER_DEVICE_FUNC_VAR(FreeDescriptorSets);
		TETHER_DEVICE_FUNC_VAR(GetDeviceQueue);
		TETHER_DEVICE_FUNC_VAR(GetPhysicalDeviceProperties);
		TETHER_DEVICE_FUNC_VAR(GetSwapchainImagesKHR);
		TETHER_DEVICE_FUNC_VAR(QueuePresentKHR);
		TETHER_DEVICE_FUNC_VAR(QueueSubmit);
		TETHER_DEVICE_FUNC_VAR(QueueWaitIdle);
		TETHER_DEVICE_FUNC_VAR(ResetCommandBuffer);
		TETHER_DEVICE_FUNC_VAR(ResetFences);
		TETHER_DEVICE_FUNC_VAR(UpdateDescriptorSets);
		TETHER_DEVICE_FUNC_VAR(WaitForFences);
	};
}
