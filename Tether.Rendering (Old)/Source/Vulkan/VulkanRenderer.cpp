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
	VulkanRenderer::VulkanRenderer(const Context& context)
		:
		m_Context(context),
		m_Dloader(m_Context.deviceLoader)
	{
		CreateAllocator();
		CreateSolidPipeline();
		CreateTexturedPipeline();
		CreateTextPipeline();
		CreateVertexBuffers();
		CreateSampler();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		m_Dloader.vkDestroySampler(m_Context.device, sampler, nullptr);

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
		CommandBufferDescriptor commandBufferDesc(commandBuffer, m_Dloader);
		for (size_t i = 0; i < objects.size(); i++)
		{
			Objects::Object* pObject = objects[i];
			if (!pObject->IsEnabled())
				continue;

			Objects::ObjectRenderer* pObjectRenderer = pObject->GetObjectRenderer();
			if (!pObjectRenderer)
				continue;

			ObjectRenderer* pRenderer = (ObjectRenderer*)pObjectRenderer;

			pRenderer->AddToCommandBuffer(commandBufferDesc, currentFrame);
		}
	}

	void VulkanRenderer::OnCreateObject(Scope<Objects::Rectangle>& object)
	{
		object = std::make_unique<Rectangle>(
			m_Context, solidPipeline.value(), square.value()
		);
	}

	void VulkanRenderer::OnCreateObject(Scope<Objects::Image>& object)
	{
		object = std::make_unique<Image>(
			m_Context, texturedPipeline.value(), square.value()
		);
	}

	void VulkanRenderer::OnCreateObject(Scope<Objects::Text>& object)
	{
		object = std::make_unique<Text>(
			m_Context, m_SwapchainExtent, textPipeline.value(), square.value()
		);
	}

	void VulkanRenderer::OnCreateResource(Scope<Resources::BufferedImage>& image,
		const Resources::BufferedImageInfo& info)
	{
		image = std::make_unique<BufferedImage>(
			m_Context, sampler,
			texturedPipelineSetLayout,
			info
		);
	}

	void VulkanRenderer::OnCreateResource(Scope<Resources::Font>& font,
		const std::string& fontPath)
	{
		font = std::make_unique<Font>(
			m_Context, 
			textPipelineLayout, sampler,
			fontPath
		);
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

	void VulkanRenderer::CreateVertexBuffers()
	{
		Math::Vector2f vertices[] =
		{
			{  0.0f,  0.0f},
			{  1.0f,  0.0f},
			{  1.0f,  1.0f},
			{  0.0f,  1.0f},
		};

		uint32_t indices[] =
		{
			0, 1, 2, 2, 3, 0
		};

		square.emplace(m_Context, sizeof(vertices), 
			sizeof(indices) / sizeof(uint32_t));
		square->UploadData(vertices, indices);
		square->FinishDataUpload();
	}

	void VulkanRenderer::CreateSampler()
	{
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_FALSE;
		samplerInfo.maxAnisotropy = 0.0f;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		if (m_Dloader.vkCreateSampler(m_Context.device, &samplerInfo, nullptr, 
			&sampler) != VK_SUCCESS)
			throw std::runtime_error("Failed to create sampler");
	}
}
