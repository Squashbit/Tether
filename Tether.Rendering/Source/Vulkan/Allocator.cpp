#include <Tether/Module/Rendering/Vulkan/Allocator.hpp>
#include <Tether/Module/Rendering/Vulkan/Common/Defs.hpp>
#include <Tether/Module/Rendering/Vulkan/GlobalVulkan.hpp>
#include <Tether/Module/Rendering/Vulkan/NativeVulkan.hpp>
#include <Tether/Module/Rendering/RendererException.hpp>

using namespace Tether::Rendering::Vulkan;

Allocator::Allocator(
	VkInstance instance,
	VkDevice device,
	VkPhysicalDevice physicalDevice,
	InstanceLoader* iloader
)
{
	VmaVulkanFunctions funcs{};
	funcs.vkGetInstanceProcAddr = TETHER_APP_VK->GetInstanceProcAddr;
	funcs.vkGetDeviceProcAddr = iloader->vkGetDeviceProcAddr;

	VmaAllocatorCreateInfo createInfo{};
	createInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	createInfo.physicalDevice = physicalDevice;
	createInfo.device = device;
	createInfo.instance = instance;
	createInfo.pVulkanFunctions = &funcs;

	if (vmaCreateAllocator(&createInfo, &allocator) != VK_SUCCESS)
		throw RendererException("VMA allocator creation failed");
}

Allocator::~Allocator()
{
	vmaDestroyAllocator(allocator);
}

VmaAllocator Allocator::Get()
{
	return allocator;
}