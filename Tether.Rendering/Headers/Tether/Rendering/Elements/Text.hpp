#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>

#include <Tether/Module/Rendering/Objects/Text.hpp>
#include <Tether/Module/Rendering/Elements/Element.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Text : public Element
	{
	public:
		using Justify = Objects::Text::Justify;

		Text(WindowUIManager& windowUI);
		
		void SetText(std::string_view text);
		void SetJustify(Justify justify);
		void SetFont(Resources::Font& font);
	private:
		void UpdateTransform() override;
		void UpdateStyle() override;

		Scope<Objects::Text> m_TextObject;
	};
}
