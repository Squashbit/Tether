#include <Tether/Module/Rendering/Vulkan/VulkanWindow.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	VulkanWindow::VulkanWindow(VulkanContext& context, VkFormat colorAttachmentFormat)
		:
		m_Device(context.device),
		m_Dloader(context.deviceLoader)
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = colorAttachmentFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentReference{};
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentReference;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkAttachmentDescription attachments[] =
		{
			colorAttachment,
		};

		VkRenderPassCreateInfo desc{};
		desc.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		desc.attachmentCount = 1;
		desc.pAttachments = attachments;
		desc.subpassCount = 1;
		desc.pSubpasses = &subpass;
		desc.dependencyCount = 1;
		desc.pDependencies = &dependency;

		if (m_Dloader.vkCreateRenderPass(m_Device, &desc, nullptr,
			&m_RenderPass) != VK_SUCCESS)
			throw std::runtime_error("Render pass creation failed");
	}

	VulkanWindow::~VulkanWindow()
	{
		m_Dloader.vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
	}

	VkRenderPass VulkanWindow::Get()
	{
		return m_RenderPass;
	}
}