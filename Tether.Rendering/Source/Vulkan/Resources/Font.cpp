#include <Tether/Rendering/Vulkan/Resources/Font.hpp>
#include <stdexcept>

namespace Tether::Rendering::Vulkan
{
	Font::CharacterCreation::CharacterCreation(ImageStager stager, 
		Character& character)
		:
		stager(std::move(stager)),
		character(character)
	{}

	Font::Font(
		GraphicsContext& context,
		VkDescriptorSetLayout setLayout,
		VkSampler sampler,
		const std::string& fontPath
	)
		:
		Resources::Font(fontPath),
		m_Device(context.GetDevice()),
		m_Dloader(context.GetDeviceLoader()),
		m_Context(context),
		m_SetLayout(setLayout),
		m_Sampler(sampler)
	{}

	Font::~Font()
	{
		m_Dloader.vkDeviceWaitIdle(m_Device);

		DisposeCharacters();
	}

	void Font::DisposeCharacters()
	{
		for (auto& [key, value] : m_Chars)
		{
			// Some characters have zero-width, so they don't have images.
			if (!value.image)
				continue;

			m_Dloader.vkDestroyDescriptorPool(m_Device, value.pool, nullptr);
			m_Dloader.vkDestroyImageView(m_Device, value.imageView, nullptr);
			vmaDestroyImage(m_Context.GetAllocator(), value.image, 
				value.imageAllocation);
		}

		m_Chars.clear();
	}

	void Font::LoadCharactersFromString(std::string_view text)
	{
		if (text.size() == 0)
			return;

		std::vector<CharacterCreation> characterCreations;

		for (size_t i = 0; i < text.size(); i++)
		{
			char c = text.at(i);

			if (!m_Chars.count(c))
				LoadChar(characterCreations, c);
		}

		for (size_t i = 0; i < characterCreations.size(); i++)
		{
			characterCreations[i].stager.Wait();
			CreateCharacterDescriptorSets(characterCreations[i].character);
		}
	}

	Font::Character& Font::GetCharacter(char c)
	{
		return m_Chars.at(c);
	}

	void Font::OnSetSize(uint32_t fontSize)
	{
		DisposeCharacters();
	}

	void Font::LoadChar(std::vector<CharacterCreation>& imageStagers, char c)
	{
		if (FT_Load_Char(m_FontFace, c, FT_LOAD_RENDER))
			return;

		FT_GlyphSlot glyph = m_FontFace->glyph;

		Character& character = m_Chars[c];
		character.size = Math::Vector2<int>(glyph->bitmap.width, glyph->bitmap.rows);
		character.bearing = Math::Vector2<int>(glyph->bitmap_left, glyph->bitmap_top);
		character.advance = glyph->advance.x;
		
		CreateCharacterImage(imageStagers, character);
	}

	void Font::CreateCharacterImage(std::vector<CharacterCreation>& imageStagers,
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
			m_Context.GetAllocator(), &imageInfo, &allocInfo,
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

		if (m_Dloader.vkCreateImageView(m_Device, &viewInfo, nullptr,
			&character.imageView) != VK_SUCCESS)
			throw std::runtime_error("Failed to create texture image view");

		ImageStager stager = ImageStager(
			m_Context,
			character.image, character.size.x, character.size.y, 1,
			m_FontFace->glyph->bitmap.buffer, imageInfo.format
		);

		CharacterCreation& creation = imageStagers.emplace_back(
			std::move(stager),
			character
		);

		creation.stager.Upload(false);
	}

	void Font::CreateCharacterDescriptorSets(Character& character)
	{
		VkDescriptorPoolSize poolSize{};
		poolSize.descriptorCount =
			static_cast<uint32_t>(m_Context.GetFramesInFlight());
		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = poolSize.descriptorCount;

		if (m_Dloader.vkCreateDescriptorPool(m_Device, &poolInfo, nullptr,
			&character.pool) != VK_SUCCESS)
			throw std::runtime_error("Failed to create descriptor pool");

		std::vector<VkDescriptorSetLayout> layouts(m_Context.GetFramesInFlight(),
			m_SetLayout);

		character.descriptorSets.resize(m_Context.GetFramesInFlight());

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorSetCount = m_Context.GetFramesInFlight();
		allocInfo.descriptorPool = character.pool;
		allocInfo.pSetLayouts = layouts.data();

		m_Dloader.vkAllocateDescriptorSets(m_Device, &allocInfo,
			character.descriptorSets.data());

		UpdateDescriptorSets(character);
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

			m_Dloader.vkUpdateDescriptorSets(m_Device, 1, &writeInfo, 0,
				nullptr);
		}
	}
}