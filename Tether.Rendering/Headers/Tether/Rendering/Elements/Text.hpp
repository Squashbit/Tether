#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Rendering/Renderer.hpp>
#include <Tether/Rendering/Resources/Font.hpp>
#include <Tether/Rendering/Elements/Element.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Text : public Element
	{
	public:
		using Justify = Renderer::TextJustify;

		Text(WindowUIManager& windowUI);
		
		void SetText(std::string_view text);
		void SetJustify(Justify justify);
		void SetFont(Resources::Font& font);
	private:
		void OnRender(Renderer& renderer) override;

		std::string m_Text;
		Justify m_Justify;
		Resources::Font* m_pFont = nullptr;
	};
}
