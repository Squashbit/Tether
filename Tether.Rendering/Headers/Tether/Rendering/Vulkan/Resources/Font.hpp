#pragma once

#include <Tether/Math/Vector2.hpp>

#include <Tether/Rendering/Resources/Font.hpp>
#include <Tether/Rendering/Vulkan/GraphicsContext.hpp>
#include <Tether/Rendering/Vulkan/ImageStager.hpp>

#include <unordered_map>
#include <vector>

namespace Tether::Rendering::Vulkan
{
	class TETHER_EXPORT Font : public Resources::Font
	{
	public:
		struct Character
		{
			VkImage image = nullptr;
			VkImageView imageView = nullptr;
			VmaAllocation imageAllocation = nullptr;
			std::vector<VkDescriptorSet> descriptorSets;

			Math::Vector2<int> size;
			Math::Vector2<int> bearing;
			uint32_t advance = 0;
		};

		Font(
			GraphicsContext& context,
			VkDescriptorSetLayout setLayout, 
			VkSampler sampler,
			const std::string& fontPath
		);
		~Font();
		TETHER_NO_COPY(Font);

		void LoadCharactersFromString(const std::string& text) override;

		Character& GetCharacter(char c);
	private:
		void DisposeCharacters();

		void RecreateDescriptorSets();
		void UpdateDescriptorSets(Character& character);

		void OnSetSize(uint32_t fontSize) override;

		void LoadChar(std::vector<ImageStager>& imageStagers, char c);

		void CreateCharacterImage(std::vector<ImageStager>& imageStagers,
			Character& character);
		
		VkDevice m_Device = nullptr;
		const DeviceLoader& m_Dloader;
		GraphicsContext& m_Context;
		
		VkDescriptorPool m_Pool = nullptr;
		VkDescriptorSetLayout m_SetLayout = nullptr;
		VkSampler m_Sampler = nullptr;

		std::unordered_map<char, Character> m_Chars;

		static constexpr const VkFormat CHARACTER_IMAGE_FORMAT = VK_FORMAT_R8_UNORM;
	};
}