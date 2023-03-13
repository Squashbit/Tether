#include <Tether/Module/Rendering/Vulkan/VulkanRenderer.hpp>
#include <Tether/Module/Rendering/Vulkan/CommandBufferDescriptor.hpp>

#include <Tether/Module/Rendering/Vulkan/Objects/Image.hpp>
#include <Tether/Module/Rendering/Vulkan/Objects/Rectangle.hpp>
#include <Tether/Module/Rendering/Vulkan/Objects/Text.hpp>

#include <Tether/Module/Rendering/Vulkan/Resources/BufferedImage.hpp>
#include <Tether/Module/Rendering/Vulkan/Resources/Font.hpp>

#include <Tether.Rendering/Assets/CompiledShaders/solid.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/solid.frag.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/text.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/text.frag.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/textured.vert.spv.h>
#include <Tether.Rendering/Assets/CompiledShaders/textured.frag.spv.h>

#include <set>

namespace Tether::Rendering::Vulkan
{
	VulkanRenderer::VulkanRenderer(const VulkanContext& context)
		:
		m_Context(context),
		m_Dloader(m_Context.deviceLoader)
	{
		TETHER_ASSERT(
			m_Context.instance != nullptr &&
			m_Context.device != nullptr &&
			m_Context.queue != nullptr &&
			m_Context.physicalDevice != nullptr &&
			m_Context.renderPass != nullptr &&
			m_Context.commandPool != nullptr &&
			m_Context.framesInFlight != 0
		);

		CreateAllocator();
		CreateSolidPipeline();
		CreateTexturedPipeline();
		CreateTextPipeline();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		m_Dloader.vkDeviceWaitIdle(m_Context.device);

		m_Dloader.vkDestroyDescriptorSetLayout(m_Context.device, texturedPipelineSetLayout,
			nullptr);
		m_Dloader.vkDestroyDescriptorSetLayout(m_Context.device, textPipelineLayout,
			nullptr);
	}

	void VulkanRenderer::SetSwapchainExtent(VkExtent2D swapchainExtent)
	{
		m_SwapchainExtent = swapchainExtent;
	}

	void VulkanRenderer::PopulateCommandBuffer(VkCommandBuffer commandBuffer)
	{
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_SwapchainExtent.width;
		viewport.height = (float)m_SwapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		m_Dloader.vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_SwapchainExtent.width;
		scissor.extent.height = m_SwapchainExtent.height;
		m_Dloader.vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		CommandBufferDescriptor commandBufferDesc(commandBuffer, m_Dloader);
		for (size_t i = 0; i < objects.size(); i++)
		{
			Objects::Object& object = *objects[i];
			if (!object.IsEnabled())
				continue;

			ObjectRenderer& renderer = (ObjectRenderer&)object.GetObjectRenderer();
			renderer.AddToCommandBuffer(commandBufferDesc, currentFrame);
		}
	}

	void VulkanRenderer::CreateAllocator()
	{
		if (m_Context.allocator)
			return;

		m_Allocator.emplace(m_Context);
	}

	void VulkanRenderer::CreateSolidPipeline()
	{
		using namespace Assets;

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(Rectangle::PushConstants);
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
			| VK_SHADER_STAGE_FRAGMENT_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		std::vector<VkVertexInputBindingDescription> bindingDescs;
		std::vector<VkVertexInputAttributeDescription> attribDescs;

		// Vector2 descriptions
		{
			VkVertexInputBindingDescription bindingDesc;
			bindingDesc.binding = 0;
			bindingDesc.stride = sizeof(Math::Vector2f);
			bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDescs.push_back(bindingDesc);

			VkVertexInputAttributeDescription posDesc;
			posDesc.binding = 0;
			posDesc.location = 0;
			posDesc.format = VK_FORMAT_R32G32_SFLOAT;
			posDesc.offset = 0;
			attribDescs.push_back(posDesc);
		}

		solidPipeline.emplace(
			m_Context, m_SwapchainExtent, 0,
			(uint32_t*)VulkanShaders::_binary_solid_vert_spv,
			sizeof(VulkanShaders::_binary_solid_vert_spv),
			(uint32_t*)VulkanShaders::_binary_solid_frag_spv,
			sizeof(VulkanShaders::_binary_solid_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}

	void VulkanRenderer::CreateTexturedPipeline()
	{
		using namespace Assets;

		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &layoutBinding;

		if (m_Dloader.vkCreateDescriptorSetLayout(m_Context.device, &layoutInfo, nullptr,
			&texturedPipelineSetLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor set layout");

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(Text::PushConstants);
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &texturedPipelineSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		std::vector<VkVertexInputBindingDescription> bindingDescs;
		std::vector<VkVertexInputAttributeDescription> attribDescs;

		// Vector2 descriptions
		{
			VkVertexInputBindingDescription bindingDesc;
			bindingDesc.binding = 0;
			bindingDesc.stride = sizeof(Math::Vector2f);
			bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDescs.push_back(bindingDesc);

			VkVertexInputAttributeDescription posDesc;
			posDesc.binding = 0;
			posDesc.location = 0;
			posDesc.format = VK_FORMAT_R32G32_SFLOAT;
			posDesc.offset = 0;
			attribDescs.push_back(posDesc);
		}

		texturedPipeline.emplace(
			m_Context, m_SwapchainExtent, 0,
			(uint32_t*)VulkanShaders::_binary_textured_vert_spv,
			sizeof(VulkanShaders::_binary_textured_vert_spv),
			(uint32_t*)VulkanShaders::_binary_textured_frag_spv,
			sizeof(VulkanShaders::_binary_textured_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}

	void VulkanRenderer::CreateTextPipeline()
	{
		using namespace Assets;

		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = 0;
		layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		layoutBinding.descriptorCount = 1;
		layoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &layoutBinding;

		if (m_Dloader.vkCreateDescriptorSetLayout(m_Context.device, &layoutInfo, nullptr,
			&textPipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor set layout");

		VkPushConstantRange pushConstantRange{};
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(Text::PushConstants);
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT 
			| VK_SHADER_STAGE_FRAGMENT_BIT;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &textPipelineLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		std::vector<VkVertexInputBindingDescription> bindingDescs;
		std::vector<VkVertexInputAttributeDescription> attribDescs;

		// Vector2 descriptions
		{
			VkVertexInputBindingDescription bindingDesc;
			bindingDesc.binding = 0;
			bindingDesc.stride = sizeof(Math::Vector2f);
			bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDescs.push_back(bindingDesc);

			VkVertexInputAttributeDescription posDesc;
			posDesc.binding = 0;
			posDesc.location = 0;
			posDesc.format = VK_FORMAT_R32G32_SFLOAT;
			posDesc.offset = 0;
			attribDescs.push_back(posDesc);
		}

		textPipeline.emplace(
			m_Context, m_SwapchainExtent, 0,
			(uint32_t*)VulkanShaders::_binary_text_vert_spv,
			sizeof(VulkanShaders::_binary_text_vert_spv),
			(uint32_t*)VulkanShaders::_binary_text_frag_spv,
			sizeof(VulkanShaders::_binary_text_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}
}
