#include <Tether/Module/Rendering/Vulkan/Resources/Font.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	Font::Font(
		Context& context,
		VkDescriptorSetLayout setLayout,
		VkSampler sampler,
		const std::string& fontPath
	)
		:
		Resources::Font(fontPath),
		m_Device(context.device),
		m_Dloader(context.deviceLoader),
		m_Context(context),
		m_SetLayout(setLayout),
		m_Sampler(sampler)
	{
		
	}

	Font::~Font()
	{
		m_Dloader.vkDeviceWaitIdle(m_Context.device);

		DisposeCharacters();

		if (m_Pool)
			m_Dloader.vkDestroyDescriptorPool(m_Context.device, m_Pool, nullptr);
	}

	void Font::DisposeCharacters()
	{
		for (auto& [key, value] : m_Chars)
		{
			// Some characters have zero-width, so they don't have images.
			if (!value.image)
				continue;

			m_Dloader.vkDestroyImageView(m_Context.device, value.imageView, nullptr);
			vmaDestroyImage(m_Context.allocator, value.image, value.imageAllocation);
		}

		m_Chars.clear();
	}

	void Font::LoadCharactersFromString(const std::string& text)
	{
		if (text.size() == 0)
			return;

		std::vector<ImageStager> imageCreations;

		for (size_t i = 0; i < text.size(); i++)
		{
			char c = text.at(i);

			if (!m_Chars.count(c))
				LoadChar(imageCreations, c);
		}

		for (size_t i = 0; i < imageCreations.size(); i++)
			imageCreations[i].Wait();

		RecreateDescriptorSets();
	}

	Font::Character& Font::GetCharacter(char c)
	{
		return m_Chars.at(c);
	}

	void Font::RecreateDescriptorSets()
	{
		m_Dloader.vkDeviceWaitIdle(m_Context.device);

		if (m_Pool)
			m_Dloader.vkDestroyDescriptorPool(m_Context.device, m_Pool, nullptr);

		VkDescriptorPoolSize poolSize{};
		poolSize.descriptorCount = 
			static_cast<uint32_t>(m_Chars.size() * m_Context.framesInFlight);
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = poolSize.descriptorCount;

		if (m_Dloader.vkCreateDescriptorPool(m_Context.device, &poolInfo, nullptr,
			&m_Pool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor pool");

		std::vector<VkDescriptorSetLayout> layouts(m_Context.framesInFlight, m_SetLayout);

		for (auto it = m_Chars.begin(); it != m_Chars.end(); it++)
		{
			Character& character = it->second;
			if (!character.image)
				continue;

			character.descriptorSets.resize(m_Context.framesInFlight);

			VkDescriptorSetAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorSetCount = m_Context.framesInFlight;
			allocInfo.descriptorPool = m_Pool;
			allocInfo.pSetLayouts = layouts.data();

			m_Dloader.vkAllocateDescriptorSets(m_Context.device, &allocInfo,
				character.descriptorSets.data());

			UpdateDescriptorSets(character);
		}
	}

	void Font::UpdateDescriptorSets(Character& character)
	{
		for (uint32_t i = 0; i < character.descriptorSets.size(); i++)
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = character.imageView;
			imageInfo.sampler = m_Sampler;
			
			VkWriteDescriptorSet writeInfo{};
			writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeInfo.dstSet = character.descriptorSets[i];
			writeInfo.dstBinding = 0;
			writeInfo.dstArrayElement = 0;
			writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeInfo.descriptorCount = 1;
			writeInfo.pImageInfo = &imageInfo;

			m_Dloader.vkUpdateDescriptorSets(m_Context.device, 1, &writeInfo, 0,
				nullptr);
		}
	}

	void Font::OnSetSize(uint32_t fontSize)
	{
		DisposeCharacters();
	}

	void Font::LoadChar(std::vector<ImageStager>& imageStagers, char c)
	{
		if (FT_Load_Char(m_FontFace, c, FT_LOAD_RENDER))
			return;

		FT_GlyphSlot glyph = m_FontFace->glyph;

		Character character;
		character.size = Math::Vector2<int>(glyph->bitmap.width, glyph->bitmap.rows);
		character.bearing = Math::Vector2<int>(glyph->bitmap_left, glyph->bitmap_top);
		character.advance = glyph->advance.x;
		
		CreateCharacterImage(imageStagers, character);
		
		m_Chars[c] = character;
	}

	void Font::CreateCharacterImage(std::vector<ImageStager>& imageStagers,
		Character& character)
	{
		// Some characters have zero-width, so they don't have images.
		if (character.size.x == 0 || character.size.y == 0)
			return;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = character.size.x;
		imageInfo.extent.height = character.size.y;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = CHARACTER_IMAGE_FORMAT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT
			| VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkResult result = vmaCreateImage(
			m_Context.allocator, &imageInfo, &allocInfo,
			&character.image, &character.imageAllocation,
			nullptr
		);

		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create image");

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = character.image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = imageInfo.format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (m_Dloader.vkCreateImageView(m_Context.device, &viewInfo, nullptr,
			&character.imageView) != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture image view");

		ImageStager& stager = imageStagers.emplace_back(
			m_Context,
			character.image, character.size.x, character.size.y, 1,
			m_FontFace->glyph->bitmap.buffer, imageInfo.format
		);

		stager.Upload(false);
	}
}