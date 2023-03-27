#include <Tether/Module/Rendering/Vulkan/Renderer.hpp>
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
	Renderer::Renderer(GraphicsContext& context, VkRenderPass renderPass)
		:
		m_Context(context),
		m_Device(m_Context.GetDevice()),
		m_Dloader(m_Context.GetDeviceLoader())
	{
		CreatePipelines(renderPass);
	}

	Renderer::Renderer(Compositor& compositor)
		:
		m_Context(compositor.m_Context),
		m_Device(m_Context.GetDevice()),
		m_Dloader(m_Context.GetDeviceLoader())
	{
		CreatePipelines(compositor.m_RenderPass);
	}

	Renderer::~Renderer()
	{
		m_Dloader.vkDeviceWaitIdle(m_Device);

		m_Dloader.vkDestroyDescriptorSetLayout(m_Device, 
			texturedPipelineSetLayout, nullptr);
		m_Dloader.vkDestroyDescriptorSetLayout(m_Device, 
			textPipelineLayout, nullptr);
	}

	void Renderer::SetViewportExtent(VkExtent2D swapchainExtent)
	{
		m_ViewportExtent = swapchainExtent;
	}

	void Renderer::PopulateCommandBuffer(VkCommandBuffer commandBuffer,
		ObjectsIter startIter, ObjectsIter endIter)
	{
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_ViewportExtent.width;
		viewport.height = (float)m_ViewportExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		m_Dloader.vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		scissor.extent.width = m_ViewportExtent.width;
		scissor.extent.height = m_ViewportExtent.height;
		m_Dloader.vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		CommandBufferDescriptor commandBufferDesc(commandBuffer, m_Dloader);
		for (ObjectsIter iter = startIter; iter != endIter; iter++)
		{
			Objects::Object* pObject = *iter;
			if (!pObject->IsEnabled())
				continue;

			ObjectRenderer& renderer = (ObjectRenderer&)pObject->GetObjectRenderer();
			renderer.AddToCommandBuffer(commandBufferDesc, currentFrame);
		}
	}

	void Renderer::CreatePipelines(VkRenderPass renderPass)
	{
		CreateSolidPipeline(renderPass);
		CreateTexturedPipeline(renderPass);
		CreateTextPipeline(renderPass);
	}

	void Renderer::CreateSolidPipeline(VkRenderPass renderPass)
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
			m_Context, m_ViewportExtent, 0,
			(uint32_t*)VulkanShaders::_binary_solid_vert_spv,
			sizeof(VulkanShaders::_binary_solid_vert_spv),
			(uint32_t*)VulkanShaders::_binary_solid_frag_spv,
			sizeof(VulkanShaders::_binary_solid_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}

	void Renderer::CreateTexturedPipeline(VkRenderPass renderPass)
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

		if (m_Dloader.vkCreateDescriptorSetLayout(m_Device, 
			&layoutInfo, nullptr, &texturedPipelineSetLayout) != VK_SUCCESS)
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
			m_Context, m_ViewportExtent, 0,
			(uint32_t*)VulkanShaders::_binary_textured_vert_spv,
			sizeof(VulkanShaders::_binary_textured_vert_spv),
			(uint32_t*)VulkanShaders::_binary_textured_frag_spv,
			sizeof(VulkanShaders::_binary_textured_frag_spv),
			bindingDescs,
			attribDescs,
			&pipelineLayoutInfo
		);
	}

	void Renderer::CreateTextPipeline(VkRenderPass renderPass)
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

		if (m_Dloader.vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr,
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
			m_Context, m_ViewportExtent, 0,
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
