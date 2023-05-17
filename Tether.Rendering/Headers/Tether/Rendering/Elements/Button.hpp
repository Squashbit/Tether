#pragma once

#include <Tether/Common/Defs.hpp>

#include <Tether/Module/Rendering/WindowUI.hpp>

#include <Tether/Module/Rendering/Objects/Text.hpp>
#include <Tether/Module/Rendering/Elements/Element.hpp>
#include <Tether/Module/Rendering/Elements/Common/BorderedElement.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT Button : public Element, public BorderedElement
	{
	public:
		Button(WindowUI& windowUI);
		~Button();

		void SetText(std::string_view text);
		void SetFont(Resources::Font& font);

		void SetOnClickFunction(std::function<void()> clickFunction);
	private:
		class ClickListener : public Input::InputListener
		{
		public:
			ClickListener(Button& button);

			void OnMouseClick(Input::MouseClickInfo& info) override;
		private:
			Button& m_Button;
		};

		void Click();

		void UpdateTransform();
		void UpdateStyle();

		Scope<Objects::Text> m_TextObject;
		
		ClickListener m_Listener;
		std::function<void()> m_ClickFunction;
	};
}
