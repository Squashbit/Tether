#include <Tether/Module/Rendering/Vulkan/Common/TypeNames.hpp>

using namespace Tether::Rendering::Vulkan;

const char* TypeNames::GetVulkanTypeAsString(VkObjectType type)
{
	switch (type)
	{
		case VK_OBJECT_TYPE_UNKNOWN:
			return "VK_OBJECT_TYPE_UNKNOWN";
		case VK_OBJECT_TYPE_INSTANCE:
			return "VK_OBJECT_TYPE_INSTANCE";
		case VK_OBJECT_TYPE_PHYSICAL_DEVICE:
			return "VK_OBJECT_TYPE_PHYSICAL_DEVICE";
		case VK_OBJECT_TYPE_DEVICE:
			return "VK_OBJECT_TYPE_DEVICE";
		case VK_OBJECT_TYPE_QUEUE:
			return "VK_OBJECT_TYPE_QUEUE";
		case VK_OBJECT_TYPE_SEMAPHORE:
			return "VK_OBJECT_TYPE_SEMAPHORE";
		case VK_OBJECT_TYPE_COMMAND_BUFFER:
			return "VK_OBJECT_TYPE_COMMAND_BUFFER";
		case VK_OBJECT_TYPE_FENCE:
			return "VK_OBJECT_TYPE_FENCE";
		case VK_OBJECT_TYPE_DEVICE_MEMORY:
			return "VK_OBJECT_TYPE_DEVICE_MEMORY";
		case VK_OBJECT_TYPE_BUFFER:
			return "VK_OBJECT_TYPE_BUFFER";
		case VK_OBJECT_TYPE_IMAGE:
			return "VK_OBJECT_TYPE_IMAGE";
		case VK_OBJECT_TYPE_EVENT:
			return "VK_OBJECT_TYPE_EVENT";
		case VK_OBJECT_TYPE_QUERY_POOL:
			return "VK_OBJECT_TYPE_QUERY_POOL";
		case VK_OBJECT_TYPE_BUFFER_VIEW:
			return "VK_OBJECT_TYPE_BUFFER_VIEW";
		case VK_OBJECT_TYPE_IMAGE_VIEW:
			return "VK_OBJECT_TYPE_IMAGE_VIEW";
		case VK_OBJECT_TYPE_SHADER_MODULE:
			return "VK_OBJECT_TYPE_SHADER_MODULE";
		case VK_OBJECT_TYPE_PIPELINE_CACHE:
			return "VK_OBJECT_TYPE_PIPELINE_CACHE";
		case VK_OBJECT_TYPE_PIPELINE_LAYOUT:
			return "VK_OBJECT_TYPE_PIPELINE_LAYOUT";
		case VK_OBJECT_TYPE_RENDER_PASS:
			return "VK_OBJECT_TYPE_RENDER_PASS";
		case VK_OBJECT_TYPE_PIPELINE:
			return "VK_OBJECT_TYPE_PIPELINE";
		case VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT:
			return "VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT";
		case VK_OBJECT_TYPE_SAMPLER:
			return "VK_OBJECT_TYPE_SAMPLER";
		case VK_OBJECT_TYPE_DESCRIPTOR_POOL:
			return "VK_OBJECT_TYPE_DESCRIPTOR_POOL";
		case VK_OBJECT_TYPE_DESCRIPTOR_SET:
			return "VK_OBJECT_TYPE_DESCRIPTOR_SET";
		case VK_OBJECT_TYPE_FRAMEBUFFER:
			return "VK_OBJECT_TYPE_FRAMEBUFFER";
		case VK_OBJECT_TYPE_COMMAND_POOL:
			return "VK_OBJECT_TYPE_COMMAND_POOL";
		case VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION:
			return "VK_OBJECT_TYPE_SAMPLER_YCBCR_CONVERSION";
		case VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE:
			return "VK_OBJECT_TYPE_DESCRIPTOR_UPDATE_TEMPLATE";
		case VK_OBJECT_TYPE_PRIVATE_DATA_SLOT:
			return "VK_OBJECT_TYPE_PRIVATE_DATA_SLOT";
		case VK_OBJECT_TYPE_SURFACE_KHR:
			return "VK_OBJECT_TYPE_SURFACE_KHR";
		case VK_OBJECT_TYPE_SWAPCHAIN_KHR:
			return "VK_OBJECT_TYPE_SWAPCHAIN_KHR";
		case VK_OBJECT_TYPE_DISPLAY_KHR:
			return "VK_OBJECT_TYPE_DISPLAY_KHR";
		case VK_OBJECT_TYPE_DISPLAY_MODE_KHR:
			return "VK_OBJECT_TYPE_DISPLAY_MODE_KHR";
		case VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT:
			return "VK_OBJECT_TYPE_DEBUG_REPORT_CALLBACK_EXT";
		case VK_OBJECT_TYPE_CU_MODULE_NVX:
			return "VK_OBJECT_TYPE_CU_MODULE_NVX";
		case VK_OBJECT_TYPE_CU_FUNCTION_NVX:
			return "VK_OBJECT_TYPE_CU_FUNCTION_NVX";
		case VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT:
			return "VK_OBJECT_TYPE_DEBUG_UTILS_MESSENGER_EXT";
		case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR:
			return "VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR";
		case VK_OBJECT_TYPE_VALIDATION_CACHE_EXT:
			return "VK_OBJECT_TYPE_VALIDATION_CACHE_EXT";
		case VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV:
			return "VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_NV";
		case VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL:
			return "VK_OBJECT_TYPE_PERFORMANCE_CONFIGURATION_INTEL";
		case VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR:
			return "VK_OBJECT_TYPE_DEFERRED_OPERATION_KHR";
		case VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV:
			return "VK_OBJECT_TYPE_INDIRECT_COMMANDS_LAYOUT_NV";
		case VK_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA:
			return "VK_OBJECT_TYPE_BUFFER_COLLECTION_FUCHSIA";
	}

	return "Unknown";
}